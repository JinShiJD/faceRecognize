#ifndef QUERYWIN_H
#define QUERYWIN_H

#include <QWidget>
#include <QSqlTableModel>

namespace Ui {
class queryWin;
}

class queryWin : public QWidget
{
    Q_OBJECT

public:
    explicit queryWin(QWidget *parent = nullptr);
    ~queryWin();

private slots:
    void on_btnQuery_clicked();

private:
    Ui::queryWin *ui;
    QSqlTableModel *model;
};

#endif // QUERYWIN_H
