#ifndef ALLPLAN_H
#define ALLPLAN_H

#include <QMainWindow>
#include<QVector>
#include "plan.h"
namespace Ui {
class Allplan;
}

class Allplan : public QMainWindow
{
    Q_OBJECT

public:
    explicit Allplan(QWidget *parent = nullptr);
    ~Allplan();
    void displays_plans(QVector<plan> plans, QVector<int> &sortplants);
    void setupUiStyle();
    void changeTheme(int index);
private:
    Ui::Allplan *ui;

};

#endif // ALLPLAN_H
