# SmokelessRuntimeEFIPatcher
The SmokelessRuntimeEFIPatcher source code is forked at v0.1.4c and will not be further improved from within this repository.
This repository provides the tools and documentation required to run SmokelessRuntimeEFIPatcher and all dependencies from source.
I've also added instructions how to extract the BIOS cap files and how it convets to the SREP configuration file.

### Requirements
 - build-essential
 - uuid-dev
 - iasl
 - git
 - nasm 
 - python3
 - EDK II (edk2-stable202608)

### Compile
Clone the EDK II repository:
```
git clone https://github.com/tianocore/edk2.git
cd edk2
git fetch --tags
git checkout tags/edk2-stable202608 -b edk2-stable202608
git submodule update --init
. ./edksetup.sh
make -C BaseTools
```

Now copy the SmokelessRuntimeEFIPatcher and SetupPkg directories to the root folder of EDK II and run the following build commands:
```
build -p ShellPkg/ShellPkg.dsc -a X64 -t GCC -b RELEASE
build -p SmokelessRuntimeEFIPatcher/SmokelessRuntimeEFIPatcher.dsc -a X64 -t GCC -b RELEASE
build -p SetupPkg/Setup.dsc -a X64 -t GCC -b RELEASE
```

The result will be three EFI files that needs to be copied to a flash drive:
```
edk2/Build/Shell/RELEASE_GCC/X64/ShellPkg/Application/Shell/Shell/OUTPUT/Shell.efi -> EFI/BOOTX64.EFI
edk2/Build/SmokelessRuntimeEFIPatcher/RELEASE_GCC/X64/SmokelessRuntimeEFIPatcher.efi -> SmokelessRuntimeEFIPatcher.efi
edk2/Build/Setup/RELEASE_GCC/X64/Setup.efi -> Setup.efi
```

### Extract BIOS data
This method may be used to extract the Setup image from recent Lenovo ThinkCentre BIOS files:
 - Download latest BIOS file
 - Retrieve https://github.com/platomav/biosutilities and run the following command:
 - `python3 ./main.py ../imageM5P.cap -e -o ../output -u AmiPfatExtract`
 - Run UEFItool and open the output file (00)
 - Now locate the Setup/PE32 image and extract the the body
 - Use `ifrextractor` to extract the form data from the Setup image
 - `./ifrextractor Section_PE32_image_Setup_Setup_body.efi`
 - A file named `Section_PE32_image_Setup_Setup_body.efi.0.1.en-US.uefi.ifr.txt` will be created with all BIOS related data

The result text file will contain all information required to make modifications by using SREP. To unlock the hidden Advanced menu, the following parts are relevant:

```
FormSet Guid: 7B59104A-C00D-4158-87FF-F04D6396A915, Title: "Setup Mode", Help: "Setup"
...
        Form FormId: 0x2710, Title: "Setup Mode"
                Ref Prompt: "Main", Help: "", QuestionFlags: 0x0, QuestionId: 0x1, VarStoreId: 0x0, VarStoreInfo: 0xFFFF, FormId: 0x2712
                Ref Prompt: "Advanced", Help: "", QuestionFlags: 0x0, QuestionId: 0x2, VarStoreId: 0x0, VarStoreInfo: 0xFFFF, FormId: 0x2716
                Ref Prompt: "Devices", Help: "", QuestionFlags: 0x0, QuestionId: 0x3, VarStoreId: 0x0, VarStoreInfo: 0xFFFF, FormId: 0x2714
                Ref Prompt: "Security", Help: "", QuestionFlags: 0x0, QuestionId: 0x4, VarStoreId: 0x0, VarStoreInfo: 0xFFFF, FormId: 0x2719
                Ref Prompt: "Power", Help: "", QuestionFlags: 0x0, QuestionId: 0x5, VarStoreId: 0x0, VarStoreInfo: 0xFFFF, FormId: 0x2717
                Ref Prompt: "Startup", Help: "", QuestionFlags: 0x0, QuestionId: 0x6, VarStoreId: 0x0, VarStoreInfo: 0xFFFF, FormId: 0x271B
                Ref Prompt: "Exit", Help: "", QuestionFlags: 0x0, QuestionId: 0x7, VarStoreId: 0x0, VarStoreInfo: 0xFFFF, FormId: 0x271D
        End
...
        Form FormId: 0x2715, Title: "Advanced"
```

For later use in the `SREP_Config.cfg` file the Guid and FormId are converted to a guid byte-order string.
 This results in the Setup Mode Guid to become `4A10597B0DC0584187FFF04D6396A915`, appended with the Main FormId `1227`.


### Configuration
The config file must be written to `SREP_Config.cfg`, when using Lenovo P3 tiny or m90q you may use the following example config:
```
Op Loaded
AMITSE
Op Patch
Pattern
4A10597B0DC0584187FFF04D6396A915122700000000000000000000000000004A10597B0DC0584187FFF04D6396A915162700000000000000000000000000004A10597B0DC0584187FFF04D6396A915142700000000000000000000000000004A10597B0DC0584187FFF04D6396A915192700000000000000000000000000004A10597B0DC0584187FFF04D6396A915172700000000000000000000000000004A10597B0DC0584187FFF04D6396A9151B2700000000000000000000000000004A10597B0DC0584187FFF04D6396A9151D27
4A10597B0DC0584187FFF04D6396A915122700000000000000000000000000004A10597B0DC0584187FFF04D6396A9151527
Op End
```

This configuration will patch the Setup Mode form 0x2710, by replacing the Advanced menu 0x2716 with the hidden Advanced menu 0x2715.

### Run
Start the computer and boot to the USB flash drive, which starts an EFI shell. Within the EFI shell you can run `map` to identify the drive.
In this example I'll be using fs1 as the USB flash drive.
```
fs1:
SmokelessRuntimeEFIPatcher.efi
Setup.efi
```

This will patch and start the native BIOS Setup program, you should now have the hidden Avanced menu unlocked.
