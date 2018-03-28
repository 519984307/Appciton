#include <QCoreApplication>
#include <QSocketNotifier>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <stdarg.h>
#include <execinfo.h>
#include <cxxabi.h>
#include <dlfcn.h>
#include <sys/socket.h>
#include "Debug.h"

#define CRASH_LOG_PATH "/usr/local/nPM/bin/crashlog"

/*******************************************************************************
 * 打印函数栈
 ******************************************************************************/
static void printStack(FILE *fp)
{
    void *trace[32];
    int size = backtrace(trace, 32);
    char **messages = backtrace_symbols(trace, size);

    for (int i = 3; i < size && messages != NULL; i++)
    {
        char *mangled_name = 0, *offset_begin = 0, *offset_end = 0;

        // find parantheses and +address offset surrounding mangled name
        for (char *p = messages[i]; *p; ++p)
        {
            if (*p == '(')
            {
                mangled_name = p;
            }
            else if (*p == '+')
            {
                offset_begin = p;
            }
            else if (*p == ')')
            {
                offset_end = p;
                break;
            }
        }

        // if the line could be processed, attempt to demangle the symbol
        if (mangled_name && offset_begin && offset_end &&
            mangled_name < offset_begin)
        {
            *mangled_name++ = '\0';
            *offset_begin++ = '\0';
            *offset_end++ = '\0';

            int status;
            char * real_name = abi::__cxa_demangle(mangled_name, 0, 0, &status);

            // if demangling is successful, output the demangled function name
            if (status == 0)
            {
                fprintf(fp, "[bt]: %s : %s +%s %s\r\n", messages[i], real_name, offset_begin, offset_end);

            }
            // otherwise, output the mangled function name
            else
            {
                fprintf(fp, "[bt]: %s : %s+%s%s\r\n", messages[i], mangled_name, offset_begin, offset_end);
            }
            free(real_name);
        }
        // otherwise, print the whole line
        else
        {
            fprintf(fp, "[bt]: %s\r\n", messages[i]);
        }
    }

    fprintf(fp, "\r\n");
}

/*******************************************************************************
 * 信号处理函数
 ******************************************************************************/
static void _fatalErrorHandler(int sig)
{
    char str[32];
    FILE *fp = NULL;
    time_t t = time(NULL);
    strftime(str, sizeof(str), "%Y-%m-%d %H:%M:%S", localtime(&t));

    flockfile (stderr);
    fprintf(stderr, "\n%s occur at %s\n", strsignal(sig), str);
    printStack(stderr);
    funlockfile(stderr);

    fp = fopen(CRASH_LOG_PATH, "w+");
    if(fp)
    {
        fprintf(fp, "\r\n%s occur at %s\r\n", strsignal(sig), str);
        printStack(fp);
        fclose(fp);
    }

    exit (EXIT_FAILURE);
}

int _fds[2];

/*******************************************************************************
 * 退出程序处理函数
 ******************************************************************************/
static void _exitHandler(int /*sig*/)
{
    static int count = 0;
    switch (++count)
    {
        case 1:
        {
            // 正常情况下，尝试通知应用程序退出
            char a = 1;
            int ret = write(_fds[0], &a, 1);
            if (ret != 1)
            {
               ret = write(_fds[0], &a, 1);
            }
            break;
        }

        case 2:
        {
            // 应用程序可能卡死了, 尝试打印出可读性较好的函数栈
            fprintf(stderr, "\nforce exit\n");
            printStack(stderr);
            raise (SIGKILL);
            break;
        }

        case 3:
        {
            // 若崩馈严重, 如调用malloc或printf都出不来, 尝试打印出可读性较差的函数栈
            int stack_depth;
            const int max_depth = 32;
            void *trace[max_depth];
            int ret = write(STDERR_FILENO, "\nforce exit again\n", 18);
            if (ret != 18)
            {
               ret = write(STDERR_FILENO, "\nforce exit again\n", 18);
            }
            stack_depth = backtrace(trace, max_depth);
            backtrace_symbols_fd(trace, stack_depth, STDERR_FILENO);
            raise (SIGKILL);
            break;
        }

        default:
            raise (SIGKILL);
            break;
    }
}

static QSocketNotifier *_notifier = NULL;
/*******************************************************************************
 * 捕捉段错误等, 自动打印函数栈
 ******************************************************************************/
void catchFatalError(void)
{
    signal(SIGSEGV, _fatalErrorHandler);
    signal(SIGILL, _fatalErrorHandler);
    signal(SIGBUS, _fatalErrorHandler);
    signal(SIGSYS, _fatalErrorHandler);
    signal(SIGFPE, _fatalErrorHandler);

    // 按一下Ctrl+C通知程序退出, 再按一下Ctrl+C强制程序退出
    if (!socketpair(AF_UNIX, SOCK_STREAM, 0, _fds))
    {
        _notifier = new QSocketNotifier(_fds[1], QSocketNotifier::Read, qApp);
        QObject::connect(_notifier, SIGNAL(activated(int)), qApp, SLOT(quit()));

        signal(SIGTERM, _exitHandler);
        signal(SIGINT, _exitHandler);
    }
}

/*******************************************************************************
 * 清理
 ******************************************************************************/
void deleteErrorCatch(void)
{
    if (_notifier != NULL)
    {
        QObject::disconnect(_notifier, SIGNAL(activated(int)), qApp, SLOT(quit()));
        delete _notifier;
    }
}
