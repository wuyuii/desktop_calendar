// qdatedialog.h
#ifndef QDATEDIALOG_H
#define QDATEDIALOG_H

#include <QDialog>
#include <QDate>
#include <QDateEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

class QDateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QDateDialog(QWidget *parent = nullptr);
    QDate selectedDate() const;

private:
    QDateEdit *dateEdit;
    QPushButton *okButton;
    QPushButton *cancelButton;
    QDate selected;

private slots:
    void onOkButtonClicked();
    void onCancelButtonClicked();
};

#endif // QDATEDIALOG_H
