/********************************************************************************
** Form generated from reading UI file 'calendar.ui'
**
** Created by: Qt User Interface Compiler version 5.12.12
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CALENDAR_H
#define UI_CALENDAR_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_calendar
{
public:
    QWidget *centralwidget;
    QWidget *widget;
    QLabel *time_label;
    QLabel *date_label;
    QWidget *gridLayoutWidget;
    QGridLayout *gridLayout;
    QLabel *selected_date_label;
    QPushButton *pre_month_pushButton;
    QPushButton *next_month_pushButton;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *calendar)
    {
        if (calendar->objectName().isEmpty())
            calendar->setObjectName(QString::fromUtf8("calendar"));
        calendar->resize(1118, 743);
        centralwidget = new QWidget(calendar);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        widget = new QWidget(centralwidget);
        widget->setObjectName(QString::fromUtf8("widget"));
        widget->setGeometry(QRect(30, 10, 581, 101));
        time_label = new QLabel(widget);
        time_label->setObjectName(QString::fromUtf8("time_label"));
        time_label->setGeometry(QRect(30, 10, 541, 31));
        date_label = new QLabel(widget);
        date_label->setObjectName(QString::fromUtf8("date_label"));
        date_label->setGeometry(QRect(30, 60, 541, 31));
        gridLayoutWidget = new QWidget(centralwidget);
        gridLayoutWidget->setObjectName(QString::fromUtf8("gridLayoutWidget"));
        gridLayoutWidget->setGeometry(QRect(20, 180, 535, 460));
        gridLayout = new QGridLayout(gridLayoutWidget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        selected_date_label = new QLabel(centralwidget);
        selected_date_label->setObjectName(QString::fromUtf8("selected_date_label"));
        selected_date_label->setGeometry(QRect(20, 130, 221, 31));
        pre_month_pushButton = new QPushButton(centralwidget);
        pre_month_pushButton->setObjectName(QString::fromUtf8("pre_month_pushButton"));
        pre_month_pushButton->setGeometry(QRect(500, 130, 41, 41));
        next_month_pushButton = new QPushButton(centralwidget);
        next_month_pushButton->setObjectName(QString::fromUtf8("next_month_pushButton"));
        next_month_pushButton->setGeometry(QRect(550, 130, 41, 41));
        calendar->setCentralWidget(centralwidget);
        menubar = new QMenuBar(calendar);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1118, 26));
        calendar->setMenuBar(menubar);
        statusbar = new QStatusBar(calendar);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        calendar->setStatusBar(statusbar);

        retranslateUi(calendar);

        QMetaObject::connectSlotsByName(calendar);
    } // setupUi

    void retranslateUi(QMainWindow *calendar)
    {
        calendar->setWindowTitle(QApplication::translate("calendar", "calendar", nullptr));
        time_label->setText(QApplication::translate("calendar", "TextLabel", nullptr));
        date_label->setText(QApplication::translate("calendar", "TextLabel", nullptr));
        selected_date_label->setText(QApplication::translate("calendar", "TextLabel", nullptr));
        pre_month_pushButton->setText(QApplication::translate("calendar", "PushButton", nullptr));
        next_month_pushButton->setText(QApplication::translate("calendar", "PushButton", nullptr));
    } // retranslateUi

};

namespace Ui {
    class calendar: public Ui_calendar {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CALENDAR_H
