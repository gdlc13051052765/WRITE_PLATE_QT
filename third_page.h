#ifndef THIRD_PAGE_H
#define THIRD_PAGE_H


#include <QWidget>
#include <QTouchEvent>
#include <QPushButton>
#include <QLabel>
#include <QSlider>




class Third_Page_Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Third_Page_Widget(QWidget *parent = 0);
    ~Third_Page_Widget();

protected:
    void paintEvent(QPaintEvent *); //重绘函数

    bool eventFilter(QObject *watched, QEvent *event);

private:

    QStringList menuList;   //菜单内容
    QFont itemFont;         //菜单字体样式
    QPushButton *Button_Back;     //返回按钮

};


#endif // WIDGET_H
