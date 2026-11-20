/*
 * Copyright (c) 2020 rxi
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include <log/log.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <direct.h>  // Windows _mkdir
#include <chrono>    // 文件日志时间戳的毫秒部分
#include <mutex>     // 内置日志锁(见 log_log 的说明)

#define MAX_CALLBACKS 32
#define MAX_LOG_PATH 260
#define MAX_FILENAME_PATTERN 256

typedef struct {
    log_LogFn fn;
    void *udata;
    int level;
} Callback;

static struct {
    void *udata;
    log_LockFn lock;
    int level;
    bool quiet;
    Callback callbacks[MAX_CALLBACKS];
    
    // 新增：文件日志配置（方案 A）
    char log_dir[MAX_LOG_PATH];           // 日志目录路径
    char filename_pattern[MAX_FILENAME_PATTERN]; // 文件名模式（如 "app_%Y-%m-%d.log"）
    bool file_enabled;                    // 文件日志启用标志
    FILE* log_fp;                         // 当前日志文件指针
    char current_log_path[MAX_LOG_PATH];  // 当前日志文件完整路径
    time_t current_day_start;             // 当前天的开始时间（用于按天轮转）
} L;

static const char *level_strings[] = {
    "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"
};

#ifdef LOG_USE_COLOR
static const char *level_colors[] = {
    "\x1b[94m", "\x1b[36m", "\x1b[32m", "\x1b[33m", "\x1b[31m", "\x1b[35m"
};
#endif

// ========== 新增：文件日志管理辅助函数（方案 A）==========

/**
 * 创建目录（如果不存在），支持递归创建多级目录
 * 支持 Windows 和 Linux
 */
static int create_dir_if_not_exists(const char* path) {
    if (path == NULL || strlen(path) == 0) {
        return -1;
    }
    
    // 检查目录是否已存在
#ifdef _WIN32
    struct _stat st;
    if (_stat(path, &st) == 0) {
        return (st.st_mode & _S_IFDIR) ? 0 : -1;  // 已存在且是目录
    }
#else
    struct stat st;
    if (stat(path, &st) == 0) {
        return S_ISDIR(st.st_mode) ? 0 : -1;
    }
#endif
    
    // 递归创建父目录
    char tmp[MAX_LOG_PATH];
    strncpy(tmp, path, MAX_LOG_PATH - 1);
    tmp[MAX_LOG_PATH - 1] = '\0';
    
    char* p = NULL;
    size_t len = strlen(tmp);
    
    // 从根目录开始逐级创建
    for (p = tmp + 1; p < tmp + len; p++) {
        // 找到路径分隔符
        if (*p == '/' || *p == '\\') {
            *p = '\0';  // 临时截断
            
            // 检查当前级目录是否存在
#ifdef _WIN32
            if (_stat(tmp, &st) != 0) {
                _mkdir(tmp);  // 创建目录
            }
#else
            if (stat(tmp, &st) != 0) {
                mkdir(tmp, 0755);
            }
#endif
            
            *p = '/';  // 恢复分隔符
        }
    }
    
    // 创建最后一级目录
#ifdef _WIN32
    return _mkdir(tmp);
#else
    return mkdir(tmp, 0755);
#endif
}

/**
 * 生成带日期的日志文件名
 * 支持格式：%Y-%m-%d, %H-%M-%S 等
 */
static void generate_log_filename(const char* pattern, time_t* t, char* output, size_t output_size) {
    if (pattern == NULL || output == NULL) {
        return;
    }
    
    struct tm* tm_info = localtime(t);
    if (tm_info == NULL) {
        strncpy(output, pattern, output_size - 1);
        output[output_size - 1] = '\0';
        return;
    }
    
    // 简单替换：%Y-%m-%d → 实际日期
    char temp[MAX_FILENAME_PATTERN];
    strncpy(temp, pattern, MAX_FILENAME_PATTERN - 1);
    temp[MAX_FILENAME_PATTERN - 1] = '\0';
    
    // 替换 %Y
    char year_buf[16];
    sprintf(year_buf, "%04d", tm_info->tm_year + 1900);
    char* pos = strstr(temp, "%Y");
    if (pos) {
        char new_temp[MAX_FILENAME_PATTERN];
        strncpy(new_temp, temp, pos - temp);
        new_temp[pos - temp] = '\0';
        strcat(new_temp, year_buf);
        strcat(new_temp, pos + 2);
        strncpy(temp, new_temp, MAX_FILENAME_PATTERN - 1);
    }
    
    // 替换 %m
    char month_buf[16];
    sprintf(month_buf, "%02d", tm_info->tm_mon + 1);
    pos = strstr(temp, "%m");
    if (pos) {
        char new_temp[MAX_FILENAME_PATTERN];
        strncpy(new_temp, temp, pos - temp);
        new_temp[pos - temp] = '\0';
        strcat(new_temp, month_buf);
        strcat(new_temp, pos + 2);
        strncpy(temp, new_temp, MAX_FILENAME_PATTERN - 1);
    }
    
    // 替换 %d
    char day_buf[16];
    sprintf(day_buf, "%02d", tm_info->tm_mday);
    pos = strstr(temp, "%d");
    if (pos) {
        char new_temp[MAX_FILENAME_PATTERN];
        strncpy(new_temp, temp, pos - temp);
        new_temp[pos - temp] = '\0';
        strcat(new_temp, day_buf);
        strcat(new_temp, pos + 2);
        strncpy(temp, new_temp, MAX_FILENAME_PATTERN - 1);
    }
    
    // 替换 %H
    char hour_buf[16];
    sprintf(hour_buf, "%02d", tm_info->tm_hour);
    pos = strstr(temp, "%H");
    if (pos) {
        char new_temp[MAX_FILENAME_PATTERN];
        strncpy(new_temp, temp, pos - temp);
        new_temp[pos - temp] = '\0';
        strcat(new_temp, hour_buf);
        strcat(new_temp, pos + 2);
        strncpy(temp, new_temp, MAX_FILENAME_PATTERN - 1);
    }
    
    // 替换 %M
    char min_buf[16];
    sprintf(min_buf, "%02d", tm_info->tm_min);
    pos = strstr(temp, "%M");
    if (pos) {
        char new_temp[MAX_FILENAME_PATTERN];
        strncpy(new_temp, temp, pos - temp);
        new_temp[pos - temp] = '\0';
        strcat(new_temp, min_buf);
        strcat(new_temp, pos + 2);
        strncpy(temp, new_temp, MAX_FILENAME_PATTERN - 1);
    }
    
    // 替换 %S
    char sec_buf[16];
    sprintf(sec_buf, "%02d", tm_info->tm_sec);
    pos = strstr(temp, "%S");
    if (pos) {
        char new_temp[MAX_FILENAME_PATTERN];
        strncpy(new_temp, temp, pos - temp);
        new_temp[pos - temp] = '\0';
        strcat(new_temp, sec_buf);
        strcat(new_temp, pos + 2);
        strncpy(temp, new_temp, MAX_FILENAME_PATTERN - 1);
    }
    
    strncpy(output, temp, output_size - 1);
    output[output_size - 1] = '\0';
}

/**
 * 构建完整的日志文件路径
 */
static void build_log_path(const char* dir, const char* filename, char* output, size_t output_size) {
    if (dir == NULL || filename == NULL || output == NULL) {
        return;
    }
    
    size_t dir_len = strlen(dir);
    size_t filename_len = strlen(filename);
    
    // 检查目录末尾是否有分隔符
    bool has_separator = (dir_len > 0) && (dir[dir_len - 1] == '/' || dir[dir_len - 1] == '\\');
    
    if (has_separator) {
        snprintf(output, output_size, "%s%s", dir, filename);
    } else {
#ifdef _WIN32
        snprintf(output, output_size, "%s\\%s", dir, filename);
#else
        snprintf(output, output_size, "%s/%s", dir, filename);
#endif
    }
}

/**
 * 检查是否需要按天轮转（新的一天开始了）
 */
static bool need_day_rotation(time_t now) {
    if (L.current_day_start == 0) {
        return true;  // 首次使用
    }

    // ⚠️ 不能比较 localtime 的两个返回值: 它们指向同一份 CRT 静态缓冲,
    //    第二次调用会把第一次的结果覆盖 ⇒ 两字段永远相等, 跨天轮转永远不触发。
    //    各自拷进独立的 struct tm 再比。
    struct tm now_tm;
    struct tm start_tm;
#ifdef _WIN32
    localtime_s(&now_tm, &now);
    localtime_s(&start_tm, &L.current_day_start);
#else
    localtime_r(&now, &now_tm);
    localtime_r(&L.current_day_start, &start_tm);
#endif

    // 检查是否是新的一天
    return (now_tm.tm_yday != start_tm.tm_yday) || (now_tm.tm_year != start_tm.tm_year);
}

/**
 * 打开或重新打开日志文件
 */
static FILE* get_or_reopen_log_file(void) {
    if (!L.file_enabled || strlen(L.log_dir) == 0 || strlen(L.filename_pattern) == 0) {
        return NULL;
    }
    
    time_t now = time(NULL);
    
    // 检查是否需要按天轮转
    if (need_day_rotation(now)) {
        if (L.log_fp) {
            fclose(L.log_fp);
            L.log_fp = NULL;
        }
        L.current_day_start = now;
    }
    
    // 如果文件已打开，直接返回
    if (L.log_fp) {
        return L.log_fp;
    }
    
    // 生成文件名
    char filename[MAX_FILENAME_PATTERN];
    generate_log_filename(L.filename_pattern, &now, filename, sizeof(filename));
    
    // 构建完整路径
    build_log_path(L.log_dir, filename, L.current_log_path, sizeof(L.current_log_path));
    
    // 打开文件（追加模式）
    L.log_fp = fopen(L.current_log_path, "a");
    if (L.log_fp) {
        // 写入文件打开标记
        char buf[64];
        time_t t = time(NULL);
        struct tm* tm_info = localtime(&t);
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tm_info);
        fprintf(L.log_fp, "\n========== 日志文件已打开：%s ==========\n", buf);
        fflush(L.log_fp);
    }
    
    return L.log_fp;
}

/**
 * 文件日志回调函数（方案 A）
 */
static void file_path_callback(log_Event *ev) {
    FILE* fp = get_or_reopen_log_file();
    if (!fp) {
        return;
    }
    
    char buf[64];
    buf[strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", ev->time)] = '\0';
    // 毫秒部分(与时区无关): 阶段耗时常常不足 1 秒, 秒级分辨率不足以做耗时分析
    const long long ms = static_cast<long long>(
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count() % 1000);
    fprintf(
        fp, "[%s.%03lld][%-5s][%s:%d %s]: ",
        buf, ms, level_strings[ev->level], ev->file, ev->line, ev->funName);
    vfprintf(fp, ev->fmt, ev->ap);
    fprintf(fp, "\n");
    fflush(fp);
}

// ========== 标准回调函数 ==========

static void stdout_callback(log_Event *ev) {
    char buf[16];
    buf[strftime(buf, sizeof(buf), "%H:%M:%S", ev->time)] = '\0';
#ifdef LOG_USE_COLOR
    fprintf(
            ev->udata, "%s %s%-5s\x1b[0m \x1b[90m%s:%d %s:\x1b[0m ",
            buf, level_colors[ev->level], level_strings[ev->level],
            ev->file, ev->line, ev->funName);
#else
    fprintf(
            (FILE *)ev->udata, "[%s][%-5s][%s:%d %s]: ",
            buf, level_strings[ev->level], ev->file, ev->line, ev->funName);
#endif
    vfprintf((FILE *)ev->udata, ev->fmt, ev->ap);
    fprintf((FILE *)ev->udata, "\n");
    fflush((FILE *)ev->udata);
}


static void file_callback(log_Event *ev) {
    char buf[64];
    buf[strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", ev->time)] = '\0';
    fprintf(
            (FILE *)ev->udata, "%s %-5s %s:%d: ",
            buf, level_strings[ev->level], ev->file, ev->line);
    vfprintf((FILE *)ev->udata, ev->fmt, ev->ap);
    fprintf((FILE *)ev->udata, "\n");
    fflush((FILE *)ev->udata);
}


static void lock(void)   {
    if (L.lock) { L.lock(true, L.udata); }
}


static void unlock(void) {
    if (L.lock) { L.lock(false, L.udata); }
}


const char* log_level_string(int level) {
    return level_strings[level];
}


void log_set_lock(log_LockFn fn, void *udata) {
    L.lock = fn;
    L.udata = udata;
}


void log_set_level(int level) {
    L.level = level;
}


void log_set_quiet(bool enable) {
    L.quiet = enable;
}


int log_add_callback(log_LogFn fn, void *udata, int level) {
    for (int i = 0; i < MAX_CALLBACKS; i++) {
        if (!L.callbacks[i].fn) {
            L.callbacks[i].fn = fn;
            L.callbacks[i].udata = udata;
            L.callbacks[i].level = level;
            return 0;
        }
    }
    return -1;
}


int log_add_fp(FILE *fp, int level) {
    return log_add_callback(file_callback, fp, level);
}


static void init_event(log_Event *ev, void *udata) {
    if (!ev->time) {
        // ⚠️ 不要把 localtime 的返回值(CRT 共享静态缓冲的指针)直接存进 ev->time:
        //    回调 file_path_callback → get_or_reopen_log_file → need_day_rotation
        //    还会再调 localtime, 把同一份缓冲覆写成"本次运行日志文件的起始时刻",
        //    于是 strftime 打出来的是启动那一秒 —— 整个日志文件的时间戳全部冻结
        //    (2026-09-25 实测踩到)。拷进本线程私有的一份, 之后谁再调 localtime 都无影响。
        static thread_local struct tm tls_tm;
        time_t t = time(NULL);
#ifdef _WIN32
        localtime_s(&tls_tm, &t);
#else
        localtime_r(&t, &tls_tm);
#endif
        ev->time = &tls_tm;
    }
    ev->udata = udata;
}


// ========== 新增：文件日志管理 API 实现 ==========

/**
 * 设置日志输出目录
 * @param dir_path 目录路径（如 "logs" 或 "D:/logs"）
 * @return 0 成功，-1 失败
 */
COMMON_DLL int log_set_log_dir(const char* dir_path) {
    if (dir_path == NULL || strlen(dir_path) == 0) {
        return -1;
    }
    
    strncpy(L.log_dir, dir_path, MAX_LOG_PATH - 1);
    L.log_dir[MAX_LOG_PATH - 1] = '\0';
    
    // 尝试创建目录
    int ret = create_dir_if_not_exists(L.log_dir);
    if (ret != 0) {
        // 目录创建失败，但继续（可能已存在）
    }
    
    return 0;
}

/**
 * 设置日志文件名模式
 * @param pattern 文件名模式（如 "app_%Y-%m-%d.log"）
 * @return 0 成功，-1 失败
 */
COMMON_DLL int log_set_filename_pattern(const char* pattern) {
    if (pattern == NULL || strlen(pattern) == 0) {
        return -1;
    }
    
    strncpy(L.filename_pattern, pattern, MAX_FILENAME_PATTERN - 1);
    L.filename_pattern[MAX_FILENAME_PATTERN - 1] = '\0';
    
    return 0;
}

/**
 * 一键启用文件日志
 * @param dir_path 日志目录
 * @param filename_pattern 文件名模式
 * @param level 日志级别
 * @return 0 成功，-1 失败
 */
COMMON_DLL int log_enable_file(const char* dir_path, const char* filename_pattern, int level) {
    if (log_set_log_dir(dir_path) != 0) {
        return -1;
    }
    if (log_set_filename_pattern(filename_pattern) != 0) {
        return -1;
    }
    
    L.file_enabled = true;
    L.current_day_start = 0;  // 触发首次打开
    L.log_fp = NULL;
    
    // 注册文件日志回调
    int ret = log_add_callback(file_path_callback, NULL, level);
    return ret;
}

/**
 * 获取当前日志目录
 */
COMMON_DLL const char* log_get_log_dir(void) {
    return L.log_dir;
}

/**
 * 检查文件日志是否启用
 */
COMMON_DLL bool log_is_file_enabled(void) {
    return L.file_enabled;
}

/**
 * 手动刷新日志文件
 */
COMMON_DLL void log_flush_file(void) {
    if (L.log_fp) {
        fflush(L.log_fp);
    }
}

// ========== 核心日志函数 ==========

/**内置的日志锁。
 *
 * 为什么必须有: 日志是**跨线程**使用的(3dxml 解析 worker / Poly 线程池 / 主线程都会写),
 * 而一条日志要分多次写(前缀 fprintf + 正文 vfprintf + 换行), 外部又从未调用 log_set_lock
 * ⇒ 两个线程的行会相互穿插成"混合行"(2026-09-26 实测抓到:
 * "[VsgGeometryLines.cpp:88 setLines]: [ERpcDispatcher.cpp:1312 handleAddItem]: VsgG...")。
 * 混合行既妨碍排障, 也会让靠 grep 行数断言的用例偶发失败。
 *
 * 口径: 本锁是兜底, 外部若用 log_set_lock 装了锁, lock()/unlock() 照旧生效(两者叠加)。
 * 用 recursive_mutex: 万一日后有回调在锁内再打日志(当前没有), 也不会自锁死。*/
static std::recursive_mutex& internal_log_mutex()
{
    static std::recursive_mutex m;
    return m;
}

void log_log(int level, const char *file, int line, const char *funName, const char *fmt, ...)
{
    std::lock_guard<std::recursive_mutex> internalLock(internal_log_mutex());

    log_Event ev;
    ev.fmt = fmt;
    ev.file = file;
    ev.line = line;
    ev.funName = funName;
    ev.level = level;

    // 提取文件名（去除路径）
    const char* fileBaseName;
    #ifdef _WIN32
    fileBaseName = strrchr(file, '\\');
    #elif __linux__
    fileBaseName = strrchr(file, '/');
    #endif
    fileBaseName = fileBaseName ? fileBaseName + 1 : file;
    ev.file = fileBaseName;

    lock();

    if (!L.quiet && level >= L.level) {
        init_event(&ev, stdout);
        va_start(ev.ap, fmt);
        stdout_callback(&ev);
        va_end(ev.ap);
    }

    for (int i = 0; i < MAX_CALLBACKS && L.callbacks[i].fn; i++) {
        Callback *cb = &L.callbacks[i];
        if (level >= cb->level) {
            init_event(&ev, cb->udata);
            va_start(ev.ap, fmt);
            cb->fn(&ev);
            va_end(ev.ap);
        }
    }

    unlock();
}
