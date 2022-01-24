#ifndef SEARCHSCENE_H
#define SEARCHSCENE_H

#include <QWidget>
#include <QDateTime>
#include <QDialog>
#include <QDebug>
#include <QScrollBar>
#include <QButtonGroup>
#include "qchartviewer.h"

namespace Ui {
class SearchScene;
class SearchCalender;
}

class QCalendarWidget;
class QCheckBox;
class QComboBox;
class QDate;
class QDateEdit;
class QGridLayout;
class QGroupBox;
class QLabel;

class SearchCalender : public QWidget
{
    Q_OBJECT
public:
    SearchCalender(QWidget *parent = nullptr);

private slots:
    void localeChanged(int index);
    void firstDayChanged(int index);
    void selectionModeChanged(int index);
    void horizontalHeaderChanged(int index);
    void verticalHeaderChanged(int index);
    void selectedDateChanged();
    void minimumDateChanged(QDate date);
    void maximumDateChanged(QDate date);
    void weekdayFormatChanged();
    void weekendFormatChanged();
    void reformatHeaders();
    void reformatCalendarPage();

private:
    void createPreviewGroupBox();
    void createGeneralOptionsGroupBox();
    void createDatesGroupBox();
    void createTextFormatsGroupBox();
    QComboBox *createColorComboBox();

    QGroupBox *previewGroupBox;
    QGridLayout *previewLayout;
    QCalendarWidget *calendar;

    QGroupBox *generalOptionsGroupBox;
    QLabel *localeLabel;
    QLabel *firstDayLabel;
//! [0]
    QLabel *selectionModeLabel;
    QLabel *horizontalHeaderLabel;
    QLabel *verticalHeaderLabel;
    QComboBox *localeCombo;
    QComboBox *firstDayCombo;
    QComboBox *selectionModeCombo;
    QCheckBox *gridCheckBox;
    QCheckBox *navigationCheckBox;
    QComboBox *horizontalHeaderCombo;
    QComboBox *verticalHeaderCombo;

    QGroupBox *datesGroupBox;
    QLabel *currentDateLabel;
    QLabel *minimumDateLabel;
    QLabel *maximumDateLabel;
    QDateEdit *currentDateEdit;
    QDateEdit *minimumDateEdit;
    QDateEdit *maximumDateEdit;

    QGroupBox *textFormatsGroupBox;
    QLabel *weekdayColorLabel;
    QLabel *weekendColorLabel;
    QLabel *headerTextFormatLabel;
    QComboBox *weekdayColorCombo;
    QComboBox *weekendColorCombo;
    QComboBox *headerTextFormatCombo;

    QCheckBox *firstFridayCheckBox;
//! [1]
    QCheckBox *mayFirstCheckBox;
};

class SearchScene : public QDialog
{
    Q_OBJECT

public:
    explicit SearchScene(QWidget *parent = nullptr);
    ~SearchScene();

private slots:
    void on_btBack_clicked();

    void on_btCalStart_clicked();

    void on_btCalEnd_clicked();

    void on_btSearch_clicked();

    void on_btExcel_clicked();

    void on_btPicture_clicked();

    void on_ckAmTemp_toggled(bool checked);

    void on_ckTgTemp_toggled(bool checked);

    void on_ckAmHumid_toggled(bool checked);

    void on_ckTgHumid_toggled(bool checked);

    void on_ckPreLamp_toggled(bool checked);

    void on_ckDO1_toggled(bool checked);

    void on_ckDO2_toggled(bool checked);

    void on_ckDO3_toggled(bool checked);

    void on_ckDO4_toggled(bool checked);

    void on_ckDO5_toggled(bool checked);

private:
    Ui::SearchScene *ui;

private:
    struct isShow
    {
        bool AmbTemp;
        bool TgtTemp;
        bool AmbHumi;
        bool TgtHumi;
        bool LampPre;
        bool DO1;
        bool DO2;
        bool DO3;
        bool DO4;
        bool DO5;
    };
    isShow showList;
    SearchCalender cal;

private:
    struct DataArray
    {
        DoubleArray m_timeStamps;
        DoubleArray AmbTemp;
        DoubleArray TgtTemp;
        DoubleArray AmbHumi;
        DoubleArray TgtHumi;
        DoubleArray LampPre;
        DoubleArray DO1;
        DoubleArray DO2;
        DoubleArray DO3;
        DoubleArray DO4;
        DoubleArray DO5;
    };
    DataArray m_dtArr;

    void drawChart();           // Draw chart
    void drawFullChart(QViewPortControl *vpc);   // Draw full chart
    void trackLineLegend(XYChart *c, int mouseX);   // Draw Track Cursor

private slots:
    void onViewPortChanged();
    void onMouseMovePlotArea(QMouseEvent *event);
};

#endif // SEARCHSCENE_H
