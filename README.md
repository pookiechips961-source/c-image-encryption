# Image Encryptor

A command-line image encryption tool written in C. Encrypts and decrypts images of any format supported by ffmpeg using XOR against a pseudorandom keystream seeded by a user-supplied integer key.

---

## Dependencies

- GCC or any C99-compatible compiler
- [ffmpeg](https://ffmpeg.org/) available in PATH

## Build

```
gcc -o encryptor main.c
```

## Usage

```
./encryptor <image> <key>
```

Encrypting an image produces a file prefixed with `encrypted_`. Running the same command on the encrypted file with the same key reverses the operation.

```
./encryptor photo.jpg 42819
./encryptor encrypted_photo.jpg.png 42819
```

The key can be any integer. The same key must be used to decrypt.

---

## How It Works

The input image is converted to a raw PPM file via ffmpeg to strip any format-specific encoding and expose the pixel bytes directly. Each byte is XORed against a value drawn from a pseudorandom sequence seeded by the key. The result is written back and re-encoded to PNG via ffmpeg. The intermediate PPM file is removed after processing.

Because XOR with the same keystream is its own inverse, encryption and decryption are the same operation.

---

## Security Notice

This tool is not cryptographically secure. It uses the C standard library `rand()`, which is not a cryptographic PRNG and produces platform-dependent output. The same key may not decrypt correctly across different operating systems or compilers. This project is intended as a demonstration, not for protecting sensitive data. Fixes maybe implemented in the future.

---

## Known Issues

- Output filename handling on decryption is unreliable for certain input filenames. To be fixed.
- No validation that the full pixel buffer was read from the PPM file.

---

## License

MIT
