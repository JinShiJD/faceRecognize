#include "querywin.h"
#include "ui_querywin.h"

queryWin::queryWin(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::queryWin)
{
    ui->setupUi(this);
    model = new QSqlTableModel();
}

queryWin::~queryWin()
{
    delete ui;
}

void queryWin::on_btnQuery_clicked()
{
    if(ui->rBtnEmployee->isChecked())
    {
        model->setTable("employee");    //设置员工表格
    }
    if(ui->rBtnAttendence->isChecked())
    {
        model->setTable("attendance");    //设置考勤表格
    }

    // 设置过滤器
    //model->setFilter("name='张三'")
    // 查询
    model->select();

    ui->tableViewInfo->setModel(model);

}
