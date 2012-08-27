#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QTextEdit>
#include <QLineEdit>
#include <QLabel>
#include <QHBoxLayout>
#include <QStringList>

class MainWindow : public QMainWindow
{
    Q_OBJECT

    QPushButton * compare,*r_format,*swap;          // pushbutton to add commands to edit
    QTextEdit * origin,*rewrited,*out;      // edit - keeps string, that we want to enter into terminal , stdout and sterr streams directed to qtextedit out
    QLineEdit *filters;
    QWidget * centralWidget;    // centralwidget
    QGridLayout *cwLayout;      // layout for the centralwidget
    QLabel *lbl_origin, *lbl_rewrited,*lbl_compare,*lbl_origin_count, *lbl_rewrited_count,*lbl_filters;




public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

public slots:
    void count_origin();
    void count_rewrited();
    void compare_strings();
    int damerau_levenshtein(QString s1, QString s2);
    void removeformat();
    void swap_texts();
};



#endif // MAINWINDOW_H
