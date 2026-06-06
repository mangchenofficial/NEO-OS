[bits 16]
[org 0x7C00]

start:
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    mov [boot_drive], dl

    call enable_a20
    call set_video_mode
    call load_kernel
    cli
    lgdt [gdt_desc]
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    jmp 0x08:protected_mode

enable_a20:
    in al, 0x64
    test al, 2
    jnz enable_a20
    mov al, 0xD1
    out 0x64, al
.w1:
    in al, 0x64
    test al, 2
    jnz .w1
    mov al, 0xDF
    out 0x60, al
.w2:
    in al, 0x64
    test al, 2
    jnz .w2
    ret

set_video_mode:
    ; 简单模式：先使用标准 VGA 320x200 8位色模式 (0x13)
    ; 后续可升级到 VBE 模式
    mov ax, 0x13
    int 0x10
    
    ; 保存模式信息到固定地址 0x2000
    mov word [0x2000], 320
    mov word [0x2002], 200
    mov byte [0x2004], 8
    mov dword [0x2005], 0xA0000  ; 320x200 帧缓冲地址
    ret

load_kernel:
    mov ah, 0x02
    mov al, 64
    mov ch, 0
    mov cl, 2
    mov dh, 0
    mov dl, [boot_drive]
    mov bx, 0x8000
    mov es, bx
    xor bx, bx
    int 0x13
    jc load_kernel
    ret

[bits 32]
protected_mode:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov fs, ax
    mov gs, ax
    mov esp, 0x90000
    mov ebp, esp

    mov edi, 0x100000
    mov esi, 0x80000
    mov ecx, (64 * 512) / 4
    rep movsd

    jmp 0x100000

gdt_start:
gdt_null: dq 0
gdt_code: dq 0x00CF9A000000FFFF
gdt_data: dq 0x00CF92000000FFFF
gdt_user_code: dq 0x00CFFA000000FFFF
gdt_user_data: dq 0x00CFF2000000FFFF
gdt_end:

gdt_desc:
    dw gdt_end - gdt_start - 1
    dd gdt_start

boot_drive db 0

times 510-($-$$) db 0
dw 0xAA55
