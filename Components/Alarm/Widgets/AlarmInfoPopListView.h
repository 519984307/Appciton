/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/10/8
 **/


#pragma once
#include <QDialog>
#include <QLabel>
#include <QScrollArea>
#include <QColor>
#include "AlarmDefine.h"

class QWidget;
class AlarmInfoLabel : public QLabel
{
public:
    AlarmInfoLabel();

    ~AlarmInfoLabel() {}

    void setBgColor(QColor color)
    {
        _bgColor = color;
    }

    void setTxtColor(QColor color)
    {
        _txtColor = color;
    }

    void setLatchMark(bool flag)
    {
        _latch = flag;
    }

    void setAcknowledgeMark(bool flag)
    {
        _acknowledge = flag;
    }

    void setPauseTime(unsigned time)
    {
        _pauseTime = time;
    }

    void setAlarmType(AlarmType type)
    {
        _type = type;
    }

    void setAlarmPriority(AlarmPriority priority)
    {
        _priority = priority;
    }

protected:
    void paintEvent(QPaintEvent */*e*/);

private:
    QColor _bgColor;
    QColor _txtColor;
    bool _latch;
    bool _acknowledge;
    unsigned _pauseTime;
    AlarmType _type;
    AlarmPriority _priority;
};

//报警回顾列表
class QVBoxLayout;
class ISpinBox;
class IButton;
class QTimer;
class AlarmInfoPopListView : public QDialog
{
    Q_OBJECT

public:
    AlarmInfoPopListView(QWidget *parent, AlarmType alarmType);
    ~AlarmInfoPopListView();

    void loadData();

signals:
    void listHide();

public slots:
    void setVisible(bool);

protected:
    void keyReleaseEvent(QKeyEvent *e);
    void keyPressEvent(QKeyEvent *e);
    void showEvent(QShowEvent *e);
    void hideEvent(QHideEvent *e);

private slots:
    void _closeSlot();
    void _pageChange(QString);

private:
    void _loadData();

private:
    QVBoxLayout *_mainLayout;
    QWidget *_parent;                 // 关联的父控件
    AlarmType _alarmType;
    int _total;
    int _totalPage;
    int _curPage;
    bool _isPressed;
    static const int _pageSize = 6;   // 弹出菜单显示的最大选项数量
    AlarmInfoLabel _label[_pageSize];
    ISpinBox *_page;
    IButton *_close;
    QTimer *_timer;
};

