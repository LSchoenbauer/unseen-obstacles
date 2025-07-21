import subprocess
import re
from os.path import join

Import("env")

def extract_addresses(backtrace_output):
    # Regular expression to match hexadecimal addresses in the backtrace
    address_pattern = re.compile(r'(0x[0-9a-fA-F]+:0x[0-9a-fA-F]+)')
    addresses = address_pattern.findall(backtrace_output)
    return addresses

def pio_addr2line(source, target, env):
    addr2line_options = "-pfiaC"
    build_dir = env.subst(env['BUILD_DIR'])
    prog_name = env.subst(env['PROGNAME'])
    print("")
    print("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~")
    print(f"Translating addresses to source code lines")
    elf_file = join(build_dir, prog_name + ".elf")
    
    print("Enter the addresses separated by space or paste the backtrace: ")
    addresses_input = input("")
    addresses = extract_addresses(addresses_input)
    
    print("")
    print("Executing:")
    print(f"xtensa-esp32-elf-addr2line {addr2line_options} -e {elf_file} {addresses}")
    print("")
    addr2line_cmd = ["xtensa-esp32-elf-addr2line", addr2line_options, "-e", elf_file] + addresses
    subprocess.run(addr2line_cmd, check=True)

env.AddCustomTarget(
    name="pio_addr2line",
    dependencies=None,
    actions=[
        "pio --version",
        "python --version",
        pio_addr2line],
    title="Address 2 Line",
    description="Run xtensa-esp32-elf-addr2line on ELF file for translating backtrace addresses to source code",
    always_build=True
)
