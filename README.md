# EEditor-open

EEditor 基础开发平台的开源内核（里程碑 1）。

本仓库当前只包含 **LibCommon 的最小可编译内核层**，用于演示 EEditor 的
「反射元数据 + EItem 对象树」底座。后续里程碑会逐步加入 `LibReflection`
（C++ 反射代码生成器）与示例程序。

## 目录结构

```
EEditor-open/
├─ CMakeLists.txt      # 顶层构建入口
├─ LICENSE             # LGPL-2.1 全文
├─ COPYING             # GPL-2.0 全文（LGPL-2.1 要求随附）
├─ NOTICE              # 版权与第三方声明
└─ LibCommon/          # 最小内核层（C++17，无 Qt/OSG 依赖）
   ├─ CMakeLists.txt
   ├─ EItem.*          # 对象树基类
   ├─ EItemMeta.*      # 类元信息（属性/方法）
   ├─ EVariant.*       # 通用属性值
   ├─ EString.*        # 字符串别名与工具
   └─ ...
```

## 构建

```bash
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

Linux / macOS：

```bash
cmake -S . -B build
cmake --build build
```

产物：`build/bin/LibCommon.dll`（或 `lib/libCommon.so` / `libCommon.dylib`）。

## 许可

本仓库代码以 **GNU Lesser General Public License v2.1 or later**
（LGPL-2.1-or-later）发布，版权归 **北京格瑞特皮软件科技有限公司** 所有。
允许闭源软件以动态链接（DLL）方式使用本库；对库本身的修改需继续以 LGPL 发布。
详见 `LICENSE`、`COPYING` 与 `NOTICE`。
