// qdatedialog.cpp
#include "QQDateDialog.h"

QDateDialog::QDateDialog(QWidget *parent) : QDialog(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    dateEdit = new QDateEdit(QDate::currentDate(), this);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    okButton = new QPushButton("确定", this);
    cancelButton = new QPushButton("取消", this);

    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    mainLayout->addWidget(dateEdit);
    mainLayout->addLayout(buttonLayout);

    connect(okButton, &QPushButton::clicked, this, &QDateDialog::onOkButtonClicked);
    connect(cancelButton, &QPushButton::clicked, this, &QDateDialog::onCancelButtonClicked);
}

QDate QDateDialog::selectedDate() const
{
    return selected;
}

void QDateDialog::onOkButtonClicked()
{
    selected = dateEdit->date();
    accept(); // 发送 accepted 信号，表示对话框被接受
}

void QDateDialog::onCancelButtonClicked()
{
    reject(); // 发送 rejected 信号，表示对话框被拒绝
}
