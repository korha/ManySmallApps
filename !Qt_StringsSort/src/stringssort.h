#ifndef STRINGSSORT_H
#define STRINGSSORT_H

#include <QtWidgets/QApplication>
#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QStyle>
#include <QtGui/QClipboard>

#ifdef __MINGW32__
#pragma GCC diagnostic warning "-Wpedantic"
#endif

class StringsSort : public QWidget
{
    Q_OBJECT
public:
    StringsSort();
    ~StringsSort();

private:
    static inline bool fCompareLess(const QString &s1, const QString &s2)
    {
        return (QString::localeAwareCompare(s1, s2) < 0);
    }
    static inline bool fCompareGreater(const QString &s1, const QString &s2)
    {
        return (QString::localeAwareCompare(s1, s2) > 0);
    }
    QPlainTextEdit *teText;
    QRadioButton *rbAZ;
    QCheckBox *chbDuplicates;
    QLabel *lblCount;

private slots:
    void slotPaste() const;
    void slotMake() const;
    void slotCopy() const;
};

#endif // STRINGSSORT_H
