#ifndef BASENUMBERCONVERTER_H
#define BASENUMBERCONVERTER_H

#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStyle>
#include <QtWidgets/QTabWidget>

#ifdef __MINGW32__
#pragma GCC diagnostic warning "-Wpedantic"
#endif

class BaseNumberConverter : public QTabWidget
{
    Q_OBJECT
public:
    BaseNumberConverter();
    ~BaseNumberConverter();

private:
    QLineEdit *leInput;
    QRadioButton *rbFrom2,
    *rbFrom8,
    *rbFrom10;
    QLineEdit *leTo2,
    *leTo8,
    *leTo10,
    *leTo16;
    QSpinBox *sbIn,
    *sbOut;
    QPlainTextEdit *teIn,
    *teOut;
    bool bLetterUp;

private slots:
    void slotChangeBase(bool bToggle);
    void slotInputEdited(const QString &strInput);
    void slotConversion();
};

#endif // BASENUMBERCONVERTER_H
