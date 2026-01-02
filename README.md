# C-Steganography: LSB Encoder/Decoder for BMP Images

Author
Name: Anuraj R

## Overview

A command-line tool written in C to hide secret data within BMP image files using the Least Significant Bit (LSB) steganography technique.
This project provides a robust implementation for both encoding and decoding secret messages, protected by a password.

## Table of Contents

- [Features](#features)
- [How It Works](#how-it-works)
- [Prerequisites](#prerequisites)
- [Compilation](#compilation)
- [Usage](#usage)
  - [Encoding](#encoding)
  - [Decoding](#decoding)
- [Project Structure](#project-structure)
- [Contributing](#contributing)

## Features

- **Encode & Decode**: Supports both hiding data in an image and extracting it.
- **Password Protection**: Uses a secret "magic string" as a password to authorize the decoding process.
- **Full Metadata Storage**: Encodes the secret file's extension and size, allowing for a perfect reconstruction of the original file upon decoding.
- **Capacity Checking**: Automatically verifies if the source image has enough space to store the secret data before encoding begins.
- **Robust Error Handling**: Provides clear, user-friendly error messages for invalid arguments, file issues, or incorrect passwords.

## How It Works

The program uses the **Least Significant Bit (LSB)** steganography method.
For each byte of secret data, its 8 bits are individually embedded into the LSB of 8 consecutive bytes of the source image's pixel data.
This modification is imperceptible to the human eye.

The data is encoded in a specific sequence for reliable retrieval:

1.  **BMP Header (54 bytes)**: Copied as-is to the output image.
2.  **Magic String (Password)**: A secret string to verify authorization for decoding.
3.  **Secret File Extension Size**: The length of the secret file's extension (e.g., 4 for `.txt`).
4.  **Secret File Extension**: The actual extension string (e.g., `.txt`).
5.  **Secret File Size**: The size of the secret file's content in bytes.
6.  **Secret File Data**: The full content of the secret file.
7.  **Remaining Image Data**: The rest of the original image data is copied over to complete the output file.

## Prerequisites

To compile and run this project, you will need:
- A C compiler, such as **GCC** (GNU Compiler Collection).

## Compilation

Navigate to the project directory (`4-SkeletonCode`) and compile the source files using the following command:

```bash
gcc -o stego main.c encode.c decode.c -Wall -Wextra
```

This will create an executable file named `stego` (or `stego.exe` on Windows).

## Usage

The program is operated via the command line, with flags to specify the desired operation.

### Encoding

To hide a secret file inside a source image, use the `-e` flag.

**Syntax:**
```bash
./stego -e <source_image.bmp> <secret_file> [output_image.bmp]
```

**Arguments:**
- `<source_image.bmp>`: The path to the BMP image to use as a cover.
- `<secret_file>`: The path to the file you want to hide.
- `[output_image.bmp]`: (Optional) The name for the output steganographed image. If not provided, it defaults to `stego.bmp`.

**Example:**
```bash
./stego -e beautiful.bmp secret.txt encoded_output.bmp
```

### Decoding

To extract a hidden file from a steganographed image, use the `-d` flag. The program will prompt you to enter the password (magic string).

**Syntax:**
```bash
./stego -d <stego_image.bmp> [output_filename]
```

**Arguments:**
- `<stego_image.bmp>`: The path to the image containing the hidden data.
- `[output_filename]`: (Optional) The base name for the extracted secret file. The correct extension will be appended automatically. If not provided, it defaults to `output`.

**Example:**
```bash
./stego -d encoded_output.bmp my_secret
```

The program will then prompt for the password:
```
Enter the magic string : #*
```
If the password is correct, a file named `my_secret.txt` will be created with the hidden content.

## Project Structure

The project is organized into several key files:

| File         | Description                                                                                             |
|--------------|---------------------------------------------------------------------------------------------------------|
| `main.c`     | The entry point of the application. Parses command-line arguments and calls the appropriate functions.  |
| `encode.c`   | Contains all logic for the encoding process, from opening files to embedding data byte-by-byte.         |
| `encode.h`   | Header file for `encode.c`. Defines the `EncodeInfo` struct and function prototypes for encoding.       |
| `decode.c`   | Contains all logic for the decoding process, including password verification and data extraction.       |
| `decode.h`   | Header file for `decode.c`. Defines the `DecodeInfo` struct and function prototypes for decoding.       |
| `types.h`    | Defines common user-defined types used across the project, such as `Status` and `OperationType` enums.  |
| `common.h`   | Defines common constants shared between modules, such as the default `MAGIC_STRING`.                    |

## Contributing

Contributions are welcome! If you would like to improve the project, please feel free to fork the repository, make your changes, and open a pull request.

Possible areas for improvement include:
- Adding support for other image formats (e.g., PNG).
- Implementing a graphical user interface (GUI).
- Enhancing security with encryption on top of steganography.

## Live Demo

| Step | Terminal Output |
|------|-----------------|
| **1. Encoding** | <img width="700" alt="Encode" src="https://github.com/user-attachments/assets/066c185b-4f20-4fcd-ac4c-84a6594f2725"> |
| **2. Encoded image** | <img width="700" alt="Decode success" src="https://github.com/user-attachments/assets/0d978b44-277f-476e-96c7-743c8c74396b"> |
| **3. Decoding** | <img width="700" alt="Decode password" src="https://github.com/user-attachments/assets/a617a6d2-d551-4ff9-bbd2-0240298d457f"> |
| **4. Extracted File** | <img width="700" alt="Secret file" src="https://github.com/user-attachments/assets/ea9a2936-9b88-4edc-bc66-a510a43003a5"> |

**Zero visual change to image, perfect secret extraction! 🔒**


