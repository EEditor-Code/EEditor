#!/usr/bin/env python3
"""pyCompileCommands.py — 生成 compile_commands.json（Ninja + MSVC）

不依赖 MinGW。脚本只负责定位 Visual Studio / Ninja / vcvars64，调用 CMake
自带的 CMAKE_EXPORT_COMPILE_COMMANDS，并把结果复制到仓库根目录的
compile_commands.json，供 clangd / cpp-tools 等使用。

用法:
    python pyCompileCommands.py
    python pyCompileCommands.py --config Release
    python pyCompileCommands.py --build
    python pyCompileCommands.py --build-dir build/compdb --output compile_commands.json

默认只配置并生成 compile_commands.json；--build 会额外执行一次 Ninja 构建
（构建 LibCommon 与 ee_tests）。
"""

import argparse
import json
import shutil
import subprocess
import sys
from pathlib import Path

try:
    sys.stdout.reconfigure(encoding="utf-8")
except Exception:
    pass

SOURCE_DIR = Path(__file__).resolve().parent
DEFAULT_BUILD_DIR = SOURCE_DIR / "build" / "compdb"
DEFAULT_OUTPUT = SOURCE_DIR / "compile_commands.json"
DEFAULT_CONFIG = "Debug"
VSWHERE = Path(r"C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe")

VS_REQUIRES = "Microsoft.VisualStudio.Component.VC.Tools.x86.x64"


def quote_batch(value):
    """把参数安全地放进 .bat 命令中。"""
    return '"' + str(value).replace('"', '""') + '"'


def run_vswhere(vswhere, args):
    try:
        result = subprocess.run(
            [str(vswhere), *args],
            capture_output=True,
            text=True,
            encoding="utf-8",
            errors="replace",
            timeout=30,
        )
    except Exception:
        return None
    if result.returncode != 0:
        return None
    return result.stdout.strip()


def find_vswhere():
    if VSWHERE.is_file():
        return VSWHERE
    found = shutil.which("vswhere")
    return Path(found) if found else None


def find_visual_studio(vswhere):
    if not vswhere:
        return None
    path = run_vswhere(vswhere, [
        "-latest",
        "-products", "*",
        "-requires", VS_REQUIRES,
        "-property", "installationPath",
    ])
    if not path:
        return None
    first = path.splitlines()[0].strip()
    return Path(first) if first else None


def find_ninja(vs_root):
    candidates = []
    found = shutil.which("ninja")
    if found:
        candidates.append(Path(found))
    if vs_root:
        candidates.append(
            vs_root / "Common7/IDE/CommonExtensions/Microsoft/CMake/Ninja/ninja.exe"
        )
    for candidate in candidates:
        if candidate.is_file():
            return candidate
    return None


def find_vcvars(vs_root):
    if not vs_root:
        return None
    candidate = vs_root / "VC/Auxiliary/Build/vcvars64.bat"
    return candidate if candidate.is_file() else None


def find_cmake(vs_root):
    found = shutil.which("cmake")
    if found:
        return Path(found)
    if vs_root:
        candidate = (
            vs_root
            / "Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/cmake.exe"
        )
        if candidate.is_file():
            return candidate
    return None


def read_cached_generator(build_dir):
    cache = build_dir / "CMakeCache.txt"
    if not cache.is_file():
        return None
    try:
        for line in cache.read_text(encoding="utf-8", errors="ignore").splitlines():
            if line.startswith("CMAKE_GENERATOR:INTERNAL="):
                return line.split("=", 1)[1].strip()
    except Exception:
        return None
    return None


def resolve_path(value):
    path = Path(value)
    if not path.is_absolute():
        path = SOURCE_DIR / path
    return path.resolve()


def write_batch(build_dir, vcvars, commands):
    build_dir.mkdir(parents=True, exist_ok=True)
    batch_path = build_dir / "_pyCompileCommands.bat"
    lines = [
        "@echo off",
        f"call {quote_batch(vcvars)} >nul",
        "if errorlevel 1 exit /b 1",
    ]
    for command in commands:
        lines.append(" ".join(quote_batch(part) for part in command))
        lines.append("if errorlevel 1 exit /b 1")
    batch_path.write_text("\n".join(lines) + "\n", encoding="mbcs")
    return batch_path


def print_dry_run(vcvars, commands):
    print("将执行:")
    print(f"  call {quote_batch(vcvars)}")
    for command in commands:
        print("  " + " ".join(quote_batch(part) for part in command))


def copy_compile_commands(build_dir, output):
    source = build_dir / "compile_commands.json"
    if not source.is_file():
        print(f"❌ 未生成 {source}")
        print("   请确认 CMake 配置成功，且构建目录使用的是 Ninja 生成器。")
        return False

    output.parent.mkdir(parents=True, exist_ok=True)
    if source.resolve() != output.resolve():
        shutil.copy2(source, output)

    try:
        data = json.loads(output.read_text(encoding="utf-8"))
        count = len(data)
    except Exception:
        count = None

    if count is None:
        print(f"✅ 已复制: {output}")
    else:
        print(f"✅ 已复制: {output}（{count} 条编译命令）")
    return True


def main():
    parser = argparse.ArgumentParser(
        description="使用 Ninja + MSVC 生成 compile_commands.json（不依赖 MinGW）",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
示例:
  python pyCompileCommands.py
  python pyCompileCommands.py --config Release
  python pyCompileCommands.py --build
  python pyCompileCommands.py --build-dir build/compdb --output compile_commands.json
""",
    )
    parser.add_argument(
        "--config",
        default=DEFAULT_CONFIG,
        choices=["Debug", "Release", "RelWithDebInfo", "MinSizeRel"],
        help=f"构建配置 (默认: {DEFAULT_CONFIG})",
    )
    parser.add_argument(
        "--build-dir",
        default=str(DEFAULT_BUILD_DIR),
        help=f"Ninja 构建目录 (默认: {DEFAULT_BUILD_DIR})",
    )
    parser.add_argument(
        "--output",
        default=str(DEFAULT_OUTPUT),
        help=f"输出文件 (默认: {DEFAULT_OUTPUT})",
    )
    build_group = parser.add_mutually_exclusive_group()
    build_group.add_argument(
        "--build",
        action="store_true",
        help="配置后执行一次 Ninja 构建（默认不构建）",
    )
    build_group.add_argument(
        "--no-build",
        action="store_true",
        help="只配置并生成 compile_commands.json（默认行为，保留兼容）",
    )
    parser.add_argument(
        "--jobs",
        type=int,
        default=None,
        help="构建并行任务数（默认使用 Ninja 自动值）",
    )
    parser.add_argument(
        "--dry-run",
        action="store_true",
        help="只打印将执行的命令，不实际配置或构建",
    )

    args = parser.parse_args()

    if not sys.platform.startswith("win"):
        print("❌ 本脚本仅支持 Windows + Visual Studio")
        return 1

    vswhere = find_vswhere()
    if not vswhere:
        print("❌ 未找到 vswhere.exe（Visual Studio Installer）")
        return 1

    vs_root = find_visual_studio(vswhere)
    if not vs_root:
        print("❌ 未找到安装了 MSVC x64 工具的 Visual Studio")
        return 1

    ninja = find_ninja(vs_root)
    if not ninja:
        print("❌ 未找到 Ninja")
        print("   请安装 Ninja，或使用 Visual Studio 自带的 Ninja")
        return 1

    vcvars = find_vcvars(vs_root)
    cmake = find_cmake(vs_root)
    if not vcvars:
        print(f"❌ 未找到 vcvars64.bat: {vs_root}")
        return 1
    if not cmake:
        print("❌ 未找到 cmake.exe")
        return 1

    build_dir = resolve_path(args.build_dir)
    output = resolve_path(args.output)

    cached_generator = read_cached_generator(build_dir)
    if cached_generator and cached_generator != "Ninja":
        print(f"❌ {build_dir} 已使用 {cached_generator} 配置")
        print("   请删除该目录后重试，或换一个 --build-dir")
        return 1

    build_dir.mkdir(parents=True, exist_ok=True)

    configure_cmd = [
        cmake,
        "-S", SOURCE_DIR,
        "-B", build_dir,
        "-G", "Ninja",
        f"-DCMAKE_MAKE_PROGRAM={ninja}",
        f"-DCMAKE_BUILD_TYPE={args.config}",
        "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON",
    ]

    commands = [configure_cmd]
    if args.build:
        build_cmd = [cmake, "--build", build_dir, "--parallel"]
        if args.jobs:
            build_cmd.append(str(args.jobs))
        commands.append(build_cmd)

    print(f"📁 源码目录：{SOURCE_DIR}")
    print(f"📂 构建目录：{build_dir}")
    print(f"⚙️  生成器：Ninja / MSVC ({args.config})")
    print(f"🛠️  Ninja：{ninja}")
    print(f"🛠️  CMake：{cmake}")
    print(f"🔨 构建：{'是（--build）' if args.build else '否（只生成索引）'}")

    if args.dry_run:
        print_dry_run(vcvars, commands)
        return 0

    batch_path = write_batch(build_dir, vcvars, commands)
    try:
        result = subprocess.call(["cmd.exe", "/d", "/s", "/c", str(batch_path)])
    except FileNotFoundError:
        print("❌ 无法启动 cmd.exe")
        result = 1
    finally:
        try:
            batch_path.unlink()
        except FileNotFoundError:
            pass

    if result != 0:
        print(f"❌ 生成失败，退出码 {result}")
        return result

    return 0 if copy_compile_commands(build_dir, output) else 1


if __name__ == "__main__":
    sys.exit(main())
