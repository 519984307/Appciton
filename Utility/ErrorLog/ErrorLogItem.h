/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/10/12
 **/



#pragma once
#include <QString>
#include <QVariantMap>
#include <QDateTime>
#include <QTextStream>
#include <QMap>
#include <QDebug>
#include <QFile>

class ErrorLogItemBase;
typedef ErrorLogItemBase *(*ErrorLogItemCreateFunc)();
class ErrorLogItemBase
{
public:
    enum
    {
        Type = 0x00
    };

    enum SystemResponeToError
    {
        SYS_RSP_REPORT,
        SYS_RSP_RETRY,
        SYS_RSP_RESET,
        SYS_RSP_DISABLE,
    };

    enum SystemState
    {
        SYS_STAT_SELFTEST,
        SYS_STAT_RUNTIME,
        SYS_STAT_MONITOR_MODE,
        SYS_STAT_PACER_MODE,
    };

    enum SubSystem
    {
        SUB_SYS_MAIN_PROCESSOR,
        SUB_SYS_PD_MODULE,
        SUB_SYS_SYSTEM_BOARD,
        SUB_SYS_E5,
        SUB_SYS_S5,
        SUB_SYS_BLMSPO2,
        SUB_SYS_N5,
        SUB_SYS_T5,
        SUB_SYS_CO2,
        SUB_SYS_RESP,
        SUB_SYS_WIFI,
        SUB_SYS_PRINTER,
    };

    ErrorLogItemBase()
    {
        content["time"] = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz");
        content["name"] = QString("");
        content["log"] = QString("");
    }

    virtual ~ErrorLogItemBase() {}

    // get the time this error happend
    QString getTime() const
    {
        return content["time"].toString();
    }

    // set datetime;
    void setTime(const QDateTime &dt)
    {
        content["time"] = dt.toString("yyyy-MM-dd HH:mm:ss.zzz");
    }

    void setName(const QString &name)
    {
        content["name"] = name;
    }

    void setLog(const QString &log)
    {
        content["log"] = log;
    }

    void setPdCommLogCache(const QVariantList &cachelist);

    void setSubSystem(SubSystem subSys);

    void setSystemState(SystemState sysState);

    void setSystemResponse(SystemResponeToError sysRsp);

    // the log is empty
    virtual bool isLogEmpty() const = 0;
    // type of this log
    virtual int type() const
    {
        return Type;
    }
    // name of this log
    virtual QString name() const
    {
        return content["name"].toString();
    }
    // output the log content
    virtual void outputInfo(QTextStream &stream) const
    {
        stream << content["name"].toString() << " log at " << content["time"].toString() << "\r\n";
    }
    // item creator map
    static  QMap<int, ErrorLogItemCreateFunc> &getItemCreatorMap();

protected:
    QVariantMap content;
    friend class ErrorLog;
};

template <typename T>
struct ErrorItemCreator
{
    static ErrorLogItemBase *create()
    {
        return new T();
    }
};

template <typename T>
struct ErrorItemRegister
{
    static void register_creator()
    {
        ErrorLogItemBase::getItemCreatorMap()[T::Type] = ErrorItemCreator<T>::create;
    }
};


#define REGISTER_ERROR_ITEM(TYPE) \
static inline __attribute((constructor)) void register_creator##TYPE() { \
    ErrorItemRegister<TYPE>::register_creator(); \
}

class ErrorLogItem : public ErrorLogItemBase
{
public:
    enum
    {
        Type = 0x01
    };

    // override
    int type() const;
    // override
    void outputInfo(QTextStream &stream) const;
    // the log is empty
    virtual bool isLogEmpty() const;
};

class CrashLogItem : public ErrorLogItem
{
public:
    enum
    {
        Type = 0x02
    };

    // override
    int type() const;
    // override
    void outputInfo(QTextStream &stream) const;
    // override
    void collect(const QString &filename);
    // the log is empty
    bool isLogEmpty() const;
};

class CriticalFaultLogItem : public ErrorLogItem
{
public:
    enum
    {
        Type = 0x03
    };

    // override
    int type() const;
};
