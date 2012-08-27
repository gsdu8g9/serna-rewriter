#include "mainwindow.h"
#include <QDebug>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    //creating static widgets and layouts
    centralWidget  = new QWidget(this);


    compare = new QPushButton(QString("Compare texts"),centralWidget);
    r_format = new QPushButton(QString("Remove formatting"),centralWidget);
    swap = new QPushButton(QString("Swap texts"),centralWidget);

    filters= new QLineEdit(centralWidget);
    origin= new QTextEdit(centralWidget);
    rewrited= new QTextEdit(centralWidget);
    out= new QTextEdit(centralWidget);
    lbl_filters=new QLabel("these worlds and similar to them will be ignored, while program are comparing texts:",centralWidget);
    lbl_origin=new QLabel("input original text here:",centralWidget);
    lbl_rewrited=new QLabel("input rewrited text here:",centralWidget);
    lbl_origin_count=new QLabel("0 symbols in original text.",centralWidget);
    lbl_rewrited_count=new QLabel("0 symbols in rewrited text.",centralWidget);
    lbl_compare=new QLabel("",centralWidget);
    cwLayout = new QGridLayout(centralWidget);


    cwLayout->addWidget(lbl_filters,0,0,1,2);
    cwLayout->addWidget(filters,1,0,1,2);
    cwLayout->addWidget(lbl_origin,2,0,1,1);
    cwLayout->addWidget(lbl_rewrited,2,1,1,1);
    cwLayout->addWidget(origin,3,0,1,1);
    cwLayout->addWidget(rewrited,3,1,1,1);
    cwLayout->addWidget(lbl_origin_count,4,0,1,1);
    cwLayout->addWidget(lbl_rewrited_count,4,1,1,1);
    cwLayout->addWidget(swap,5,0,1,1);
    cwLayout->addWidget(r_format,5,1,1,1);
    cwLayout->addWidget(out,6,0,1,2);
    cwLayout->addWidget(lbl_compare,7,1,1,1);
    cwLayout->addWidget(compare,7,0,1,1);
    setCentralWidget(centralWidget);
    centralWidget->setAttribute(Qt::WA_AlwaysShowToolTips,true);

    /*origin->setMinimumHeight(400);
    origin->setMinimumWidth(400);
    rewrited->setMinimumWidth(400);
    out->setMinimumHeight(200);
    out->setMaximumHeight(300);//*/

    connect(origin,SIGNAL(textChanged()),this,SLOT(count_origin()));
    connect(rewrited,SIGNAL(textChanged()),this,SLOT(count_rewrited()));
    connect(compare,SIGNAL(clicked()),this,SLOT(compare_strings()));
    connect(r_format,SIGNAL(clicked()),this,SLOT(removeformat()));
    connect(swap,SIGNAL(clicked()),this,SLOT(swap_texts()));
    connect(filters,SIGNAL(returnPressed()),this,SLOT(compare_strings()));

}
//count chars in origin text box
void MainWindow::count_origin(){
    QTextEdit *tmp=(QTextEdit*)sender();
    lbl_origin_count->setText(QString::number(tmp->toPlainText().count())+" symbols in original text.");
}
//count chars in rewrited char box
void MainWindow::count_rewrited(){
    QTextEdit *tmp=(QTextEdit*)sender();
    lbl_rewrited_count->setText(QString::number(tmp->toPlainText().count())+" symbols in rewrited text.");
}
//signal is emiites, when compare button is clicked, main function, compares 2 texts
void MainWindow::compare_strings(){

    QString str_origin=origin->toPlainText();
    QString str_rewrited=rewrited->toPlainText();
    // removing characters like these: ,./?!@#$!%!#
    //str_origin.at(i).isPunct()
    str_origin.remove(QRegExp("<[^>]*>"));
    str_rewrited.remove(QRegExp("<[^>]*>"));
    QString str_punct=";:,.?!%#\n\r()";
    for (int i=0;i<str_punct.count();i++){
        str_origin.replace(str_punct.at(i)," ");
        str_rewrited.replace(str_punct.at(i)," ");
    }
    str_punct="«»\"'<>*-";
    for (int i=0;i<str_punct.count();i++){
        str_origin.replace(str_punct.at(i),"");
        str_rewrited.replace(str_punct.at(i),"");
    }
    //splitting text by " " into a string list
    QStringList list_origin=str_origin.split(" ",QString::SkipEmptyParts,Qt::CaseInsensitive);
    QStringList list_rewrited=str_rewrited.split(" ",QString::SkipEmptyParts,Qt::CaseInsensitive);
    //removing short words and repeated words
    foreach (QString i,list_origin)
        if (i.count()<4) list_origin.removeOne(i);
    foreach (QString i,list_rewrited)
        if (i.count()<4) list_rewrited.removeOne(i);
    //qDebug()<<"Origin:\n"<<list_origin;
    //qDebug()<<"Rewrited:\n"<<list_rewrited;

    //applying selected filters
    QStringList list_filters=filters->text().split(",",QString::SkipEmptyParts);
    for (int i=0;i<list_filters.count();i++){
        for (int j=0;j<list_origin.count();j++){
            if ((damerau_levenshtein(list_filters.at(i),list_origin.at(j))<3)){
                list_origin.removeAt(j);
                j--;
            }
        }
    }
    for (int i=0;i<list_filters.count();i++){
        for (int j=0;j<list_rewrited.count();j++){
            if ((damerau_levenshtein(list_filters.at(i),list_rewrited.at(j))<3)){
                list_rewrited.removeAt(j);
                j--;
            }
        }
    }

    //counts amount of similarities in original and rewrited texts
    //making list of pairs <number of hits, string of names of every hit>
    typedef QPair<int,QString> pair_t;

    pair_t* pair_origin= new pair_t[list_origin.count()];
    for (int i=0;i<list_origin.count();i++) pair_origin[i]=(qMakePair(0,"for word "+list_origin.at(i)+":"));
    //QList<pair_t> pair_rewrited;
    for (int i=0;i<list_origin.count();i++){
        for (int j=0;j<list_rewrited.count();j++){
            if ((damerau_levenshtein(list_origin.at(i),list_rewrited.at(j))<3)){
                pair_origin[i].first++;
                int percent=(100*(list_rewrited.at(j).count()-damerau_levenshtein(list_origin.at(i),list_rewrited.at(j))))/list_rewrited.at(j).count();
                percent+=(100*(list_origin.at(i).count()-damerau_levenshtein(list_origin.at(i),list_rewrited.at(j))))/list_origin.at(i).count();
                percent/=2;
                pair_origin[i].second=pair_origin[i].second+" "+list_rewrited.at(j)+" - "
                        +QString::number(percent)+"%;";
                //pair_rewrited;
                //qDebug()<<"i,j,hits,string"<<i<<j<<pair_origin[i].first<<pair_origin[i].second;
            }


        }
    }
    out->clear();
    int hits=0;
    for (int i=0;i<list_origin.count();i++){
        if (pair_origin[i].first!=0){
            //hits+=pair_origin[i].first;
            if (lbl_origin->text().endsWith("original text here:"))
                    out->append("In original text found "+QString::number(pair_origin[i].first)+" similarities "+pair_origin[i].second);
            else out->append("In rewrited text found "+QString::number(pair_origin[i].first)+" similarities "+pair_origin[i].second);
        }
    }




    //it seems, that removing duplicates and similar entries reduces precision of comparing texts, so i'll comment this lines
    // though  shame, loops seems elegant :(

    list_origin.removeDuplicates();
    //removing similar elements in the list (similarity measured by damerau_levenshtein (less then 3 for now)
    for (int i=0;i<list_origin.count();i++){
        for (int j=i+1;j<list_origin.count();j++){
            if ((damerau_levenshtein(list_origin.at(i),list_origin.at(j))<3))
                if (!list_origin.isEmpty()){
                    list_origin.removeAt(j);
                    j--;
                    //qDebug()<< list_origin.count()<<"i: "<<i<<"j: "<<j+1<<"list origin at i "<<list_origin.at(i)<<"list origin at j "<<list_origin.at(j);
                }

        }
    }//*/
    list_rewrited.removeDuplicates();
    //removing similar words in rewrited
    for (int i=0;i<list_rewrited.count();i++){
        for (int j=i+1;j<list_rewrited.count();j++){
            if ((damerau_levenshtein(list_rewrited.at(i),list_rewrited.at(j))<3))
                if (!list_rewrited.isEmpty()){
                    list_rewrited.removeAt(j);
                    j--;
                    //qDebug()<< list_rewrited.count()<<"i: "<<i<<"j: "<<j+1<<"list rewrited at i "<<list_rewrited.at(i)<<"list  rewrited at j "<<list_rewrited.at(j);
                }

        }
    }//*/

    int *lbl= new int[list_origin.count()];
    for (int i=0;i<list_origin.count();i++) lbl[i]=0;
    //QList<pair_t> pair_rewrited;
    for (int i=0;i<list_origin.count();i++)
        for (int j=0;j<list_rewrited.count();j++)
            if ((damerau_levenshtein(list_origin.at(i),list_rewrited.at(j))<3))
                lbl[i]++;

    for (int i=0;i<list_origin.count();i++)
        if (lbl[i]!=0) hits++;

    for (int i=0;i<list_rewrited.count();i++) list_origin.append(list_rewrited.at(i));

    list_origin.removeDuplicates();
    //removing similar elements in the list (similarity measured by damerau_levenshtein (less then 3 for now)
    for (int i=0;i<list_origin.count();i++){
        for (int j=i+1;j<list_origin.count();j++){
            if ((damerau_levenshtein(list_origin.at(i),list_origin.at(j))<3))
                if (!list_origin.isEmpty()){
                    list_origin.removeAt(j);
                    j--;
                    //qDebug()<< list_origin.count()<<"i: "<<i<<"j: "<<j+1<<"list origin at i "<<list_origin.at(i)<<"list origin at j "<<list_origin.at(j);
                }

        }
    }//*/

    //list_origin.count();
    //list_rewrited.count();
    double res=(double)(hits)/(double)(list_origin.count())/0.01;
    if (list_origin.count()) lbl_compare->setText("Texts are similar by "
                                                  +QString::number(res,'f',2)+"%");
    delete [] pair_origin;
    delete [] lbl;

}
void MainWindow::removeformat(){
    origin->setText(origin->toPlainText().remove(QRegExp("<[^>]*>")));
    rewrited->setText(rewrited->toPlainText().remove(QRegExp("<[^>]*>")));
}
void MainWindow::swap_texts(){
    QString temp_str=rewrited->toPlainText();
    rewrited->setText(origin->toPlainText());
    origin->setText(temp_str);
    temp_str=lbl_rewrited->text();
    lbl_rewrited->setText(lbl_origin->text());
    lbl_origin->setText(temp_str);
}

int MainWindow::damerau_levenshtein(QString s1, QString s2){
// *nothing to do here*
// upd: really, nothing, by some miracle it works for now, so let it be,
// *char magic*
    int lenstr1=s1.length(),lenstr2=s2.length();
    int cost=0,tmp1=0,tmp2=0,tmp11=0,tmp22=0;
    int **d=new int*[lenstr1+1];
    for(int i=0;i<lenstr1+1;i++) d[i]=new int[lenstr2+1];

    for (int i=0;i<lenstr1+1;i++) for (int j=0;j<lenstr2+1;j++) d[i][j]=0;

    for (int i=0;i<lenstr1;i++)  d[i][lenstr2]=i+1;
    for (int j=0;j<lenstr2;j++)  d[lenstr1][j]=j+1;
    //d[lenstr1][lenstr2]=0;


    for (int i=0;i<lenstr1;i++){
        for (int j=0;j<lenstr2;j++){
            if (s1.at(i)==s2.at(j)) cost=0;
            else cost=1;

            if (i-1<0) tmp1=lenstr1;
            else tmp1=i-1;

            if (j-1<0) tmp2=lenstr2;
            else tmp2=j-1;

            d[i][j]=qMin(qMin(
                             d[tmp1][j]+1, //deletion
                             d[i][tmp2]+1), //insertion
                             d[tmp1][tmp2]+cost //substitution
                             );

            if (i-2<0) tmp11=lenstr1-2+i;
            else tmp11=i-2;

            if (j-2<0) tmp22=lenstr2-2+j;
            else tmp22=j-2;

            if (i && j && (s1.at(i-1)==s2.at(j)) && (s1.at(i)==s2.at(j-1)))
            d[i][j]=qMin(d[i][j],d[tmp11][tmp22]+cost); //transposition
        }

    }

    int res=d[lenstr1-1][lenstr2-1];

    for(int i=0;i<lenstr1+1;i++) delete [] d[i];
    delete [] d;
    return res;
}

MainWindow::~MainWindow(){
    destroy(true,true);

}//*/
