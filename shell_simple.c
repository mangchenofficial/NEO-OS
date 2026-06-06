void shell_execute(char* cmd, char* output, int max_len) {
    if (!cmd || !output || max_len <= 0) return;
    
    output[0] = '\0';
    int pos = 0;

    while (*cmd == ' ' && pos < max_len - 1) cmd++;
    if (*cmd == '\0') return;

    if (strncmp(cmd, "help", 4) == 0 && (cmd[4] == ' ' || cmd[4] == '\0')) {
        strcat_str(output, &pos, max_len, "help, clear, echo, about, meminfo, time, ls, cat, run");
    } else if (strncmp(cmd, "about", 5) == 0 && (cmd[5] == ' ' || cmd[5] == '\0')) {
        strcat_str(output, &pos, max_len, "NEO OS v0.1.0");
    } else if (strncmp(cmd, "meminfo", 7) == 0 && (cmd[7] == ' ' || cmd[7] == '\0')) {
        u32 total = memory_get_total();
        u32 free = memory_get_free_pages();
        u32 used = memory_get_used_pages();
        strcat_str(output, &pos, max_len, "Total: ");
        strcat_uint(output, &pos, max_len, total / 1024 / 1024);
        strcat_str(output, &pos, max_len, " MB, Free: ");
        strcat_uint(output, &pos, max_len, free);
        strcat_str(output, &pos, max_len, " pages, Used: ");
        strcat_uint(output, &pos, max_len, used);
        strcat_str(output, &pos, max_len, " pages");
    } else if (strncmp(cmd, "time", 4) == 0 && (cmd[4] == ' ' || cmd[4] == '\0')) {
        u32 ticks = irq_get_ticks();
        strcat_str(output, &pos, max_len, "Ticks: ");
        strcat_uint(output, &pos, max_len, ticks);
        strcat_str(output, &pos, max_len, " (");
        strcat_uint(output, &pos, max_len, ticks / 18);
        strcat_str(output, &pos, max_len, " sec)");
    } else if (strncmp(cmd, "ls", 2) == 0 && (cmd[2] == ' ' || cmd[2] == '\0')) {
        vfs_dirent_t entries[32];
        size_t count;
        if (vfs_list_dir(entries, 32, &count) == 0) {
            for (size_t i = 0; i < count && i < 5; i++) {
                strcat_str(output, &pos, max_len, entries[i].name);
                strcat_str(output, &pos, max_len, " ");
                strcat_uint(output, &pos, max_len, entries[i].size);
                if (i < count - 1) strcat_str(output, &pos, max_len, ", ");
            }
        } else {
            strcat_str(output, &pos, max_len, "Failed to list directory");
        }
    } else {
        strcat_str(output, &pos, max_len, "Unknown: ");
        strcat_str(output, &pos, max_len, cmd);
    }

    output[pos] = '\0';
}
