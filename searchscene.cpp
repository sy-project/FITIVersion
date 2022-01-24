#include "searchscene.h"
#include "ui_searchscene.h"
#include <QCalendarWidget>
#include <QCheckBox>
#include <QComboBox>
#include <QDateEdit>
#include <QGridLayout>
#include <QFileDialog>
#include <QGroupBox>
#include <QLabel>
#include <QLocale>
#include <QTextCharFormat>
#include <QApplication>
#include <QPushButton>
#include <QMouseEvent>
#include <math.h>
#include <sstream>
#include <algorithm>

#include "dbcontrol.h"
#include "framework.h"

bool isStartCal;
QString StartDate;
QString EndDate;
Ui::SearchScene* SearchUI;

std::vector<DBDataType> searchData;

SearchScene::SearchScene(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SearchScene)
{
    searchData.clear();
    ui->setupUi(this);
    SearchUI = ui;
    this->setWindowTitle("Search");
    isStartCal = true;

    QPixmap BG;
    BG.load(":/Resource/Resource/BG_Search.png");
    ui->label->setPixmap(BG.scaled(1920,1080));
    ui->btBack->setStyleSheet("border-image:url(:/Resource/Resource/BT_Back.jpg)");
    ui->btSearch->setStyleSheet("border-image:url(:/Resource/Resource/BT_SearchStart.png)");
    ui->btPicture->setStyleSheet("border-image:url(:/Resource/Resource/BT_PicSave.png)");
    ui->btExcel->setStyleSheet("border-image:url(:/Resource/Resource/BT_ExcelSave.png)");

    connect(ui->graph, SIGNAL(viewPortChanged()), SLOT(onViewPortChanged()));
    connect(ui->graph, SIGNAL(mouseMovePlotArea(QMouseEvent*)),
        SLOT(onMouseMovePlotArea(QMouseEvent*)));
    showList.AmbTemp = true;
    showList.AmbHumi = true;
    showList.TgtTemp = true;
    showList.TgtHumi = true;
    showList.LampPre = true;
    showList.DO1 = true;
    showList.DO2 = true;
    showList.DO3 = true;
    showList.DO4 = true;
    showList.DO5 = true;
}

SearchScene::~SearchScene()
{
    delete ui;
}

void SearchScene::on_btBack_clicked()
{
    this->close();
}


void SearchScene::on_btCalStart_clicked()
{
    isStartCal = true;
    if(!cal.isActiveWindow())
        cal.show();
}


void SearchScene::on_btCalEnd_clicked()
{
    isStartCal = false;
    if(!cal.isActiveWindow())
        cal.show();
}


void SearchScene::on_btSearch_clicked()
{//viewportcontroldemo
    searchData.clear();
    int id = 0;

    if(StartDate == "")
        return;

    QStringList tempList = StartDate.split(" ");

    int syear = tempList.at(2).toInt();
    int smonth = tempList.at(0).toInt();
    int sday = tempList.at(1).toInt();

    if(EndDate == "")
        return;

    tempList = EndDate.split(" ");

    int eyear = tempList.at(2).toInt();
    int emonth = tempList.at(0).toInt();
    int eday = tempList.at(1).toInt();

    qDebug() << "시간 초기화";

    /*
     * 6자리일 경우
     * hhmmss
     * 5자리일 경우
     * hhmss
     * 4자리일 경우
     * hmss
    */
    struct _Time
    {
        DBDataType db;
        int hour;
        int min;
        int sec;
    };
    _Time st;

    std::vector<_Time> tempv;
    tempv.clear();
    qDebug() << syear << ":" << smonth << ":" << sday;
    qDebug() << eyear << ":" << emonth << ":" << eday;

    for(int y = syear; y <= eyear; y++)
    {
        for(int m = smonth; m <= emonth; m++)
        {
            for(int d = sday; d <= eday; d++)
            {
                DBControl::Get()->Load(searchData, id, MAIN_DATA, y, m, d);
            }
        }
    }

    if(searchData.size() == 0)
    {
        qDebug() << "DB 없음";
        return;
    }

    qDebug() << "DB 불러오기";
    const unsigned int N = searchData.size();
    for(unsigned int i = 0; i < N; i++)
    {
        if(searchData.at(i).mainDT.TIME.size() <= 2)
        {
            st.hour = 0;
            st.min = 0;
            st.sec = stoi(searchData.at(i).mainDT.TIME);
        }
        if(searchData.at(i).mainDT.TIME.size() == 3)
        {
            st.hour = 0;
            st.min = stoi(searchData.at(i).mainDT.TIME.substr(0,1));
            st.sec = stoi(searchData.at(i).mainDT.TIME.substr(1,2));
        }
        if(searchData.at(i).mainDT.TIME.size() == 4)
        {
            st.hour = 0;
            st.min = stoi(searchData.at(i).mainDT.TIME.substr(0,2));
            st.sec = stoi(searchData.at(i).mainDT.TIME.substr(2,2));
        }
        if(searchData.at(i).mainDT.TIME.size() == 5)
        {
            st.hour = stoi(searchData.at(i).mainDT.TIME.substr(0,1));
            st.min = stoi(searchData.at(i).mainDT.TIME.substr(1,2));
            st.sec = stoi(searchData.at(i).mainDT.TIME.substr(3,2));
        }
        if(searchData.at(i).mainDT.TIME.size() == 6)
        {
            st.hour = stoi(searchData.at(i).mainDT.TIME.substr(0,2));
            st.min = stoi(searchData.at(i).mainDT.TIME.substr(2,2));
            st.sec = stoi(searchData.at(i).mainDT.TIME.substr(4,2));
        }

        st.db.Table = MAIN_DATA;
        st.db.mainDT.NO = searchData.at(i).mainDT.NO;
        st.db.mainDT.YEAR = searchData.at(i).mainDT.YEAR;
        st.db.mainDT.DATEM = searchData.at(i).mainDT.DATEM;
        st.db.mainDT.DATED = searchData.at(i).mainDT.DATED;
        st.db.mainDT.TEMP_AMB = searchData.at(i).mainDT.TEMP_AMB;
        st.db.mainDT.TEMP_TGT = searchData.at(i).mainDT.TEMP_TGT;
        st.db.mainDT.HUMI_AMB = searchData.at(i).mainDT.HUMI_AMB;
        st.db.mainDT.HUMI_TGT = searchData.at(i).mainDT.HUMI_TGT;
        st.db.mainDT.LMP_PRE = searchData.at(i).mainDT.LMP_PRE;
        st.db.mainDT.DO1 = searchData.at(i).mainDT.DO1;
        st.db.mainDT.DO2 = searchData.at(i).mainDT.DO2;
        st.db.mainDT.DO3 = searchData.at(i).mainDT.DO3;
        st.db.mainDT.DO4 = searchData.at(i).mainDT.DO4;
        st.db.mainDT.DO5 = searchData.at(i).mainDT.DO5;
        st.db.mainDT.WATTS = searchData.at(i).mainDT.WATTS;
        qDebug() << st.hour << ":" << st.min << ":" << st.sec << "[" << st.db.mainDT.TEMP_AMB << "/" << st.db.mainDT.TEMP_TGT << "]";
        tempv.push_back(st);
    }
    qDebug() << "시간";

    /*
     * 연도
     * 월
     * 일
     * 시
     * 분
     * 초
     * return day1 > day2 || (day1 == day2 && (hr1 > hr2 || (hr1 == hr2 && (min1 > min2 || (min1 == min2 && (sec1 > sec2))))));
    */
    //std::sort(tempv.begin(), tempv.end(), [](const _Time &st1, const _Time &st2) {
    //    return stoi(st1.db.mainDT.YEAR) < stoi(st2.db.mainDT.YEAR) ||
    //            (stoi(st1.db.mainDT.YEAR) == stoi(st2.db.mainDT.YEAR) &&
    //            (stoi(st1.db.mainDT.DATEM) < stoi(st2.db.mainDT.DATEM) ||
    //            (stoi(st1.db.mainDT.DATEM) == stoi(st2.db.mainDT.DATEM) &&
    //            (stoi(st1.db.mainDT.DATED) < stoi(st2.db.mainDT.DATED) ||
    //            (stoi(st1.db.mainDT.DATED) == stoi(st2.db.mainDT.DATED) &&
    //            (st1.hour < st2.hour ||
    //            (st1.hour == st2.hour &&
    //            (st1.min < st2.min ||
    //            (st1.min == st2.min &&
    //            (st1.sec < st2.sec)))))))))); });
    //qDebug() << "정렬";

    qDebug() << tempv.size();
    double AmbientTemperature[tempv.size()];
    double AmbientHumidity[tempv.size()];
    double TargetTemperature[tempv.size()];
    double TargetHumidity[tempv.size()];
    double LampPreset[tempv.size()];
    double DO1[tempv.size()];
    double DO2[tempv.size()];
    double DO3[tempv.size()];
    double DO4[tempv.size()];
    double DO5[tempv.size()];
    double Time[tempv.size()];

    for(int i = 0; i < tempv.size(); i++)
    {
        AmbientTemperature[i] = tempv.at(i).db.mainDT.TEMP_AMB / 10;
        AmbientHumidity[i] = tempv.at(i).db.mainDT.HUMI_AMB / 10;
        TargetTemperature[i] = tempv.at(i).db.mainDT.TEMP_TGT / 10;
        TargetHumidity[i] = tempv.at(i).db.mainDT.HUMI_TGT / 10;

        if(AmbientTemperature[i] > 120)
            AmbientTemperature[i] = 120;
        if(TargetTemperature[i] > 120)
            TargetTemperature[i] = 120;
        if(AmbientTemperature[i] < -20)
            AmbientTemperature[i] = -20;
        if(TargetTemperature[i] < -20)
            TargetTemperature[i] = -20;

        if(AmbientHumidity[i] > 100)
            AmbientHumidity[i] = 100;
        if(TargetHumidity[i] > 100)
            TargetHumidity[i] = 100;
        if(AmbientHumidity[i] < 0)
            AmbientHumidity[i] = 0;
        if(TargetHumidity[i] < 0)
            TargetHumidity[i] = 0;

        LampPreset[i] = tempv.at(i).db.mainDT.WATTS / 10;
        DO1[i] = tempv.at(i).db.mainDT.DO1;
        if(DO1[i] != 0)
            DO1[i] = 100;
        DO2[i] = tempv.at(i).db.mainDT.DO2;
        if(DO2[i] != 0)
            DO2[i] = 100;
        DO3[i] = tempv.at(i).db.mainDT.DO3;
        if(DO3[i] != 0)
            DO3[i] = 100;
        DO4[i] = tempv.at(i).db.mainDT.DO4;
        if(DO4[i] != 0)
            DO4[i] = 100;
        DO5[i] = tempv.at(i).db.mainDT.DO5;
        if(DO5[i] != 0)
            DO5[i] = 100;
        Time[i] = Chart::chartTime(stoi(tempv.at(i).db.mainDT.YEAR), stoi(tempv.at(i).db.mainDT.DATEM), stoi(tempv.at(i).db.mainDT.DATED)
                                   , tempv.at(i).hour, tempv.at(i).min, tempv.at(i).sec);//tempv.at(i).db.mainDT.DO5;
    }
    qDebug() << "데이터 형식 적용";

    m_dtArr.m_timeStamps = DoubleArray(Time, tempv.size());
    m_dtArr.AmbTemp = DoubleArray(AmbientTemperature, tempv.size());
    qDebug() << "1";
    m_dtArr.AmbHumi = DoubleArray(AmbientHumidity, tempv.size());
    qDebug() << "2";
    m_dtArr.TgtTemp = DoubleArray(TargetTemperature, tempv.size());
    qDebug() << "3";
    m_dtArr.TgtHumi = DoubleArray(TargetHumidity, tempv.size());
    qDebug() << "4";
    m_dtArr.LampPre = DoubleArray(LampPreset, tempv.size());
    qDebug() << "5";
    m_dtArr.DO1 = DoubleArray(DO1, tempv.size());
    qDebug() << "6";
    m_dtArr.DO2 = DoubleArray(DO2, tempv.size());
    qDebug() << "7";
    m_dtArr.DO3 = DoubleArray(DO3, tempv.size());
    qDebug() << "8";
    m_dtArr.DO4 = DoubleArray(DO4, tempv.size());
    qDebug() << "9";
    m_dtArr.DO5 = DoubleArray(DO5, tempv.size());
    qDebug() << "10";

    ui->graph->setFullRange("x", m_dtArr.m_timeStamps[0], m_dtArr.m_timeStamps[m_dtArr.m_timeStamps.len - 1]);
    qDebug() << "11";

    ui->graph->setViewPortWidth(0.2);
    qDebug() << "12";
    ui->graph->setViewPortLeft(1 - ui->graph->getViewPortWidth());
    qDebug() << "13";

    ui->graph->setZoomInWidthLimit(10.0 / m_dtArr.m_timeStamps.len);
    qDebug() << "14";
    ui->graph->setMouseWheelZoomRatio(1.1);
    qDebug() << "15";

    ui->graph->updateViewPort(true, true);

    qDebug() << "차트 그리기 시작";
    XYChart* c = new XYChart(1722, 120);
    qDebug() << "차트 영역";
    c->setPlotArea(130, 5, c->getWidth() - 200, c->getHeight() - 11, 0xb5b5b5, -1, 0xb5b5b5, Chart::Transparent, 0xffffff);
    c->xAxis()->setColors(Chart::Transparent);
    qDebug() << "글자";
    c->xAxis()->setLabelStyle("Arial", 10);

    c->xAxis()->setLabelGap(-1);
    c->xAxis()->setLabelAlignment(1);

    c->yAxis()->setColors(Chart::Transparent, Chart::Transparent);

    LineLayer *layer = c->addLineLayer();
    layer->setFastLineMode();

    qDebug() << "데이터셋";
    layer->setXData(m_dtArr.m_timeStamps);
    layer->addDataSet(m_dtArr.AmbTemp, 0xff3333);
    layer->addDataSet(m_dtArr.AmbHumi, 0x3333ff);
    layer->addDataSet(m_dtArr.TgtTemp, 0xff0000);
    layer->addDataSet(m_dtArr.TgtHumi, 0x0000ff);
    layer->addDataSet(m_dtArr.LampPre, 0xffd400);
    layer->addDataSet(m_dtArr.DO1, 0x00ff00);
    layer->addDataSet(m_dtArr.DO2, 0x00ff00);
    layer->addDataSet(m_dtArr.DO3, 0x00ff00);
    layer->addDataSet(m_dtArr.DO4, 0x00ff00);
    layer->addDataSet(m_dtArr.DO5, 0x00ff00);

    qDebug() << "1";
    c->xAxis()->setDateScale(ui->graph->getValueAtViewPort("x", 0), ui->graph->getValueAtViewPort("x", 1));
    c->xAxis()->setTickDensity(75);
    c->yAxis()->setRounding(false, false);
    qDebug() << "차트 띄우기";
    ui->viewport->setChart(c);
    qDebug() << "1";
    ui->viewport->setViewer(ui->graph);
    qDebug() << "1";
}


void SearchScene::on_btExcel_clicked()
{
    if(searchData.size() > 0)
    {
        QDateTime exportDate = QDateTime::currentDateTime();
        DBControl::Get()->Export(searchData, exportDate.date().year(), exportDate.date().month(), exportDate.date().day());
    }
}


void SearchScene::on_btPicture_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save", "chartdirector_demo",
        "PNG (*.png);;JPG (*.jpg);;GIF (*.gif);;BMP (*.bmp);;SVG (*.svg);;PDF (*.pdf)");

    if (!fileName.isEmpty())
    {
        // Save the chart
        BaseChart *c = ui->graph->getChart();
        if (0 != c)
            c->makeChart(fileName.toUtf8().constData());
    }
}


void SearchScene::on_ckAmTemp_toggled(bool checked)
{
    showList.AmbTemp = checked;
    if (searchData.size()>0)
        drawChart();
}


void SearchScene::on_ckTgTemp_toggled(bool checked)
{
    showList.TgtTemp = checked;
    if (searchData.size()>0)
        drawChart();
}


void SearchScene::on_ckAmHumid_toggled(bool checked)
{
    showList.AmbHumi = checked;
    if (searchData.size()>0)
        drawChart();
}


void SearchScene::on_ckTgHumid_toggled(bool checked)
{
    showList.TgtHumi = checked;
    if (searchData.size()>0)
        drawChart();
}


void SearchScene::on_ckPreLamp_toggled(bool checked)
{
    showList.LampPre = checked;

    if (searchData.size()>0)
        drawChart();
}


void SearchScene::on_ckDO1_toggled(bool checked)
{
    showList.DO1 = checked;
    if (searchData.size()>0)
        drawChart();
}


void SearchScene::on_ckDO2_toggled(bool checked)
{
    showList.DO2 = checked;
    if (searchData.size()>0)
        drawChart();
}


void SearchScene::on_ckDO3_toggled(bool checked)
{
    showList.DO3 = checked;
    if (searchData.size()>0)
        drawChart();
}


void SearchScene::on_ckDO4_toggled(bool checked)
{
    showList.DO4 = checked;
    if (searchData.size()>0)
        drawChart();
}


void SearchScene::on_ckDO5_toggled(bool checked)
{
    showList.DO5 = checked;
    if (searchData.size()>0)
        drawChart();
}

void SearchScene::drawChart()
{
    // Get the start date and end date that are visible on the chart.
    double viewPortStartDate = ui->graph->getValueAtViewPort("x", ui->graph->getViewPortLeft());
    double viewPortEndDate = ui->graph->getValueAtViewPort("x", ui->graph->getViewPortLeft() +
        ui->graph->getViewPortWidth());

    // Get the array indexes that corresponds to the visible start and end dates
    int startIndex = (int)floor(Chart::bSearch(m_dtArr.m_timeStamps, viewPortStartDate));
    int endIndex = (int)ceil(Chart::bSearch(m_dtArr.m_timeStamps, viewPortEndDate));
    int noOfPoints = endIndex - startIndex + 1;

    // Extract the part of the data array that are visible.

    //여기서 버그나는듯싶음
    DoubleArray viewPortTimeStamps = DoubleArray(m_dtArr.m_timeStamps.data + startIndex, noOfPoints);
    DoubleArray AmbTemp = DoubleArray(m_dtArr.AmbTemp.data + startIndex, noOfPoints);
    DoubleArray TgtTemp = DoubleArray(m_dtArr.TgtTemp.data + startIndex, noOfPoints);
    DoubleArray AmbHumi = DoubleArray(m_dtArr.AmbHumi.data + startIndex, noOfPoints);
    DoubleArray TgtHumi = DoubleArray(m_dtArr.TgtHumi.data + startIndex, noOfPoints);
    DoubleArray LampPre = DoubleArray(m_dtArr.LampPre.data + startIndex, noOfPoints);
    DoubleArray DO1 = DoubleArray(m_dtArr.DO1.data + startIndex, noOfPoints);
    DoubleArray DO2 = DoubleArray(m_dtArr.DO2.data + startIndex, noOfPoints);
    DoubleArray DO3 = DoubleArray(m_dtArr.DO3.data + startIndex, noOfPoints);
    DoubleArray DO4 = DoubleArray(m_dtArr.DO4.data + startIndex, noOfPoints);
    DoubleArray DO5 = DoubleArray(m_dtArr.DO5.data + startIndex, noOfPoints);

    //
    // At this stage, we have extracted the visible data. We can use those data to plot the chart.
    //

    ///////////////////////////////////////////////////////////////////////////////////////
    // Configure overall chart appearance.
    ///////////////////////////////////////////////////////////////////////////////////////

    // Create an XYChart object of size 640 x 400 pixels
    XYChart *c = new XYChart(1722, 531);

    // Set the plotarea at (55, 55) with width 80 pixels less than chart width, and height 92 pixels
    // less than chart height. Use a vertical gradient from light blue (f0f6ff) to sky blue (a0c0ff)
    // as background. Set border to transparent and grid lines to white (ffffff).
    c->setPlotArea(130, 55, c->getWidth() - 200, c->getHeight() - 92, c->linearGradientColor(0, 55, 0,
        c->getHeight() - 35, 0xf0f6ff, 0xa0c0ff), -1, Chart::Transparent, 0xffffff, 0xffffff);

    // As the data can lie outside the plotarea in a zoomed chart, we need enable clipping.
    c->setClipping();

    // Add a title box using dark grey (0x333333) 15pt Arial Bold font

    // Set legend icon style to use line style icon, sized for 10pt font
    c->getLegend()->setLineStyleKey();
    c->getLegend()->setFontSize(10);

    // Set the x and y axis stems to transparent and the label font to 10pt Arial
    c->xAxis()->setColors(Chart::Transparent);
    c->yAxis()->setColors(Chart::Transparent);
    c->xAxis()->setLabelStyle("Arial", 10);
    c->yAxis()->setLabelStyle("Arial", 10);

    // Add axis title using 10pt Arial Bold font
    c->yAxis()->setTitle("Ionic Temperature (C)", "Arial Bold", 10);

    ///////////////////////////////////////////////////////////////////////////////////////
    // Add data to chart
    ///////////////////////////////////////////////////////////////////////////////////////

    //
    // In this example, we represent the data by lines. You may modify the code below to use other
    // representations (areas, scatter plot, etc).
    //

    // Add a line layer for the lines, using a line width of 2 pixels
    LineLayer *layer = c->addLineLayer();
    layer->setLineWidth(2);

    // In this demo, we do not have too many data points. In real code, the chart may contain a lot
    // of data points when fully zoomed out - much more than the number of horizontal pixels in this
    // plot area. So it is a good idea to use fast line mode.
    layer->setFastLineMode();

    // Now we add the 3 data series to a line layer, using the color red (0xff3333), green
    // (0x008800) and blue (0x3333cc)
    layer->setXData(viewPortTimeStamps);
    if(showList.AmbTemp)
        layer->addDataSet(AmbTemp, 0xff3333, "Ambient Temperature");
    if(showList.AmbHumi)
        layer->addDataSet(AmbHumi, 0x3333ff, "Ambient Humidity");
    if(showList.TgtTemp)
        layer->addDataSet(TgtTemp, 0xff0000, "Target Temperature");
    if(showList.TgtHumi)
        layer->addDataSet(TgtHumi, 0x0000ff, "Target Humidity");
    if(showList.LampPre)
        layer->addDataSet(LampPre, 0xffd400, "Lamp Watts");
    if(showList.DO1)
        layer->addDataSet(DO1, 0x00ff00, "DO1");
    if(showList.DO2)
        layer->addDataSet(DO2, 0x00ff00, "DO2");
    if(showList.DO3)
        layer->addDataSet(DO3, 0x00ff00, "DO3");
    if(showList.DO4)
        layer->addDataSet(DO4, 0x00ff00, "DO4");
    if(showList.DO5)
        layer->addDataSet(DO5, 0x00ff00, "DO5");

    ///////////////////////////////////////////////////////////////////////////////////////
    // Configure axis scale and labelling
    ///////////////////////////////////////////////////////////////////////////////////////

    // Set the x-axis as a date/time axis with the scale according to the view port x range.
    ui->graph->syncDateAxisWithViewPort("x", c->xAxis());

    // For the automatic y-axis labels, set the minimum spacing to 30 pixels.
    c->yAxis()->setTickDensity(30);

    //
    // In this demo, the time range can be from a few years to a few days. We demonstrate how to
    // set up different date/time format based on the time range.
    //

    // If all ticks are yearly aligned, then we use "yyyy" as the label format.
    c->xAxis()->setFormatCondition("align", 360 * 86400);
    c->xAxis()->setLabelFormat("{value|yyyy}");

    // If all ticks are monthly aligned, then we use "mmm yyyy" in bold font as the first
    // label of a year, and "mmm" for other labels.
    c->xAxis()->setFormatCondition("align", 30 * 86400);
    c->xAxis()->setMultiFormat(Chart::StartOfYearFilter(), "<*font=bold*>{value|mmm<*br*>yyyy}",
        Chart::AllPassFilter(), "{value|mmm}");

    // If all ticks are daily algined, then we use "mmm dd<*br*>yyyy" in bold font as the
    // first label of a year, and "mmm dd" in bold font as the first label of a month, and
    // "dd" for other labels.
    c->xAxis()->setFormatCondition("align", 86400);
    c->xAxis()->setMultiFormat(Chart::StartOfYearFilter(),
        "<*block,halign=left*><*font=bold*>{value|mmm dd<*br*>yyyy}",
        Chart::StartOfMonthFilter(), "<*font=bold*>{value|mmm dd}");
    c->xAxis()->setMultiFormat(Chart::AllPassFilter(), "{value|dd}");

    // For all other cases (sub-daily ticks), use "hh:nn<*br*>mmm dd" for the first label of
    // a day, and "hh:nn" for other labels.
    c->xAxis()->setFormatCondition("else");
    c->xAxis()->setMultiFormat(Chart::StartOfDayFilter(), "<*font=bold*>{value|hh:nn<*br*>mmm dd}",
        Chart::AllPassFilter(), "{value|hh:nn}");

    ///////////////////////////////////////////////////////////////////////////////////////
    // Output the chart
    ///////////////////////////////////////////////////////////////////////////////////////

    // We need to update the track line too. If the mouse is moving on the chart (eg. if
    // the user drags the mouse on the chart to scroll it), the track line will be updated
    // in the MouseMovePlotArea event. Otherwise, we need to update the track line here.
    if (!ui->graph->isInMouseMoveEvent())
    {
        trackLineLegend(c, (0 == ui->graph->getChart()) ? c->getPlotArea()->getRightX() :
            ui->graph->getPlotAreaMouseX());
    }
    delete ui->graph->getChart();
    ui->graph->setChart(c);
}

void SearchScene::trackLineLegend(XYChart *c, int mouseX)
{
    // Clear the current dynamic layer and get the DrawArea object to draw on it.
    DrawArea *d = c->initDynamicLayer();

    // The plot area object
    PlotArea *plotArea = c->getPlotArea();

    // Get the data x-value that is nearest to the mouse, and find its pixel coordinate.
    double xValue = c->getNearestXValue(mouseX);
    int xCoor = c->getXCoor(xValue);

    // Draw a vertical track line at the x-position
    d->vline(plotArea->getTopY(), plotArea->getBottomY(), xCoor, 0xaaaaaa);

    // Container to hold the legend entries
    std::vector<std::string> legendEntries;

    // Iterate through all layers to build the legend array
    for (int i = 0; i < c->getLayerCount(); ++i) {
        Layer *layer = c->getLayerByZ(i);

        // The data array index of the x-value
        int xIndex = layer->getXIndexOf(xValue);

        // Iterate through all the data sets in the layer
        for (int j = 0; j < layer->getDataSetCount(); ++j) {
            DataSet *dataSet = layer->getDataSetByZ(j);

            // We are only interested in visible data sets with names
            const char *dataName = dataSet->getDataName();
            int color = dataSet->getDataColor();
            if (dataName && *dataName && (color != Chart::Transparent)) {
                // Build the legend entry, consist of the legend icon, name and data value.
                double dataValue = dataSet->getValue(xIndex);
                std::ostringstream legendEntry;
                legendEntry << "<*block*>" << dataSet->getLegendIcon() << " " << dataName << ": " <<
                    ((dataValue == Chart::NoValue) ? "N/A" : c->formatValue(dataValue, "{value|P4}"))
                    << "<*/*>";
                legendEntries.push_back(legendEntry.str());

                // Draw a track dot for data points within the plot area
                int yCoor = c->getYCoor(dataSet->getPosition(xIndex), dataSet->getUseYAxis());
                if ((yCoor >= plotArea->getTopY()) && (yCoor <= plotArea->getBottomY())) {
                    d->circle(xCoor, yCoor, 5, 5, color, color);
                }
            }
        }
    }

    // Create the legend by joining the legend entries
    std::ostringstream legendText;
    legendText << "<*block,maxWidth=" << plotArea->getWidth() << "*><*block*><*font=Arial Bold*>["
        << c->xAxis()->getFormattedLabel(xValue, "mm/dd/yyyy") << "]<*/*>";
    for (int i = ((int)legendEntries.size()) - 1; i >= 0; --i)
        legendText << "        " << legendEntries[i];

    // Display the legend on the top of the plot area
    TTFText *t = d->text(legendText.str().c_str(), "Arial Bold", 10);
    t->draw(plotArea->getLeftX() + 5, plotArea->getTopY() - 3, 0x333333, Chart::BottomLeft);
    t->destroy();
}

void SearchScene::onViewPortChanged()
{
    if (ui->graph->needUpdateChart())
        drawChart();
}

void SearchScene::onMouseMovePlotArea(QMouseEvent *event)
{
    trackLineLegend((XYChart *)ui->graph->getChart(), ui->graph->getPlotAreaMouseX());
    ui->graph->updateDisplay();
}


SearchCalender::SearchCalender(QWidget *parent)
    : QWidget(parent)
{
    createPreviewGroupBox();
    createGeneralOptionsGroupBox();
    createDatesGroupBox();
    createTextFormatsGroupBox();

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(previewGroupBox, 0, 0);
    layout->addWidget(generalOptionsGroupBox, 0, 1);
    layout->addWidget(datesGroupBox, 1, 0);
    layout->addWidget(textFormatsGroupBox, 1, 1);
    layout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(layout);

    previewLayout->setRowMinimumHeight(0, calendar->sizeHint().height());
    previewLayout->setColumnMinimumWidth(0, calendar->sizeHint().width());

    setWindowTitle(tr("Calendar"));
    setAttribute(Qt::WA_ShowWithoutActivating);
}

void SearchCalender::localeChanged(int index)
{
    const QLocale newLocale(localeCombo->itemData(index).toLocale());
    calendar->setLocale(newLocale);
    int newLocaleFirstDayIndex = firstDayCombo->findData(newLocale.firstDayOfWeek());
    firstDayCombo->setCurrentIndex(newLocaleFirstDayIndex);
}

void SearchCalender::firstDayChanged(int index)
{
    calendar->setFirstDayOfWeek(Qt::DayOfWeek(
                                    firstDayCombo->itemData(index).toInt()));
}

void SearchCalender::selectionModeChanged(int index)
{
    calendar->setSelectionMode(QCalendarWidget::SelectionMode(
                                   selectionModeCombo->itemData(index).toInt()));
}

void SearchCalender::horizontalHeaderChanged(int index)
{
    calendar->setHorizontalHeaderFormat(QCalendarWidget::HorizontalHeaderFormat(
                                            horizontalHeaderCombo->itemData(index).toInt()));
}

void SearchCalender::verticalHeaderChanged(int index)
{
    calendar->setVerticalHeaderFormat(QCalendarWidget::VerticalHeaderFormat(
                                          verticalHeaderCombo->itemData(index).toInt()));
}

void SearchCalender::selectedDateChanged()
{
    currentDateEdit->setDate(calendar->selectedDate());
    if(isStartCal)
    {
        StartDate = currentDateEdit->text();
        SearchUI->btCalStart->setText(StartDate);
    }
    else
    {
        EndDate = currentDateEdit->text();
        SearchUI->btCalEnd->setText(EndDate);
    }
}

void SearchCalender::minimumDateChanged(QDate date)
{
    calendar->setMinimumDate(date);
    maximumDateEdit->setDate(calendar->maximumDate());
}

void SearchCalender::maximumDateChanged(QDate date)
{
    calendar->setMaximumDate(date);
    minimumDateEdit->setDate(calendar->minimumDate());
}

void SearchCalender::weekdayFormatChanged()
{
    QTextCharFormat format;

    format.setForeground(qvariant_cast<QColor>(
        weekdayColorCombo->itemData(weekdayColorCombo->currentIndex())));
    calendar->setWeekdayTextFormat(Qt::Monday, format);
    calendar->setWeekdayTextFormat(Qt::Tuesday, format);
    calendar->setWeekdayTextFormat(Qt::Wednesday, format);
    calendar->setWeekdayTextFormat(Qt::Thursday, format);
    calendar->setWeekdayTextFormat(Qt::Friday, format);
}

void SearchCalender::weekendFormatChanged()
{
    QTextCharFormat format;

    format.setForeground(qvariant_cast<QColor>(
        weekendColorCombo->itemData(weekendColorCombo->currentIndex())));
    calendar->setWeekdayTextFormat(Qt::Saturday, format);
    calendar->setWeekdayTextFormat(Qt::Sunday, format);
}

void SearchCalender::reformatHeaders()
{
    QString text = headerTextFormatCombo->currentText();
    QTextCharFormat format;

    if (text == tr("Bold"))
        format.setFontWeight(QFont::Bold);
    else if (text == tr("Italic"))
        format.setFontItalic(true);
    else if (text == tr("Green"))
        format.setForeground(Qt::green);
    calendar->setHeaderTextFormat(format);
}

void SearchCalender::reformatCalendarPage()
{
    QTextCharFormat mayFirstFormat;
    const QDate mayFirst(calendar->yearShown(), 5, 1);

    QTextCharFormat firstFridayFormat;
    QDate firstFriday(calendar->yearShown(), calendar->monthShown(), 1);
    while (firstFriday.dayOfWeek() != Qt::Friday)
        firstFriday = firstFriday.addDays(1);

    if (firstFridayCheckBox->isChecked()) {
        firstFridayFormat.setForeground(Qt::blue);
    } else { // Revert to regular colour for this day of the week.
        Qt::DayOfWeek dayOfWeek(static_cast<Qt::DayOfWeek>(firstFriday.dayOfWeek()));
        firstFridayFormat.setForeground(calendar->weekdayTextFormat(dayOfWeek).foreground());
    }

    calendar->setDateTextFormat(firstFriday, firstFridayFormat);

    // When it is checked, "May First in Red" always takes precedence over "First Friday in Blue".
    if (mayFirstCheckBox->isChecked()) {
        mayFirstFormat.setForeground(Qt::red);
    } else if (!firstFridayCheckBox->isChecked() || firstFriday != mayFirst) {
        // We can now be certain we won't be resetting "May First in Red" when we restore
        // may 1st's regular colour for this day of the week.
        Qt::DayOfWeek dayOfWeek(static_cast<Qt::DayOfWeek>(mayFirst.dayOfWeek()));
        calendar->setDateTextFormat(mayFirst, calendar->weekdayTextFormat(dayOfWeek));
    }

    calendar->setDateTextFormat(mayFirst, mayFirstFormat);
}

void SearchCalender::createPreviewGroupBox()
{
    previewGroupBox = new QGroupBox(tr("Preview"));

    calendar = new QCalendarWidget;
    calendar->setMinimumDate(QDate(1900, 1, 1));
    calendar->setMaximumDate(QDate(3000, 1, 1));
    calendar->setGridVisible(true);

    connect(calendar, &QCalendarWidget::currentPageChanged,
            this, &SearchCalender::reformatCalendarPage);

    previewLayout = new QGridLayout;
    previewLayout->addWidget(calendar, 0, 0, Qt::AlignCenter);
    previewGroupBox->setLayout(previewLayout);
}

void SearchCalender::createGeneralOptionsGroupBox()
{
    generalOptionsGroupBox = new QGroupBox(tr("General Options"));

    localeCombo = new QComboBox;
    int curLocaleIndex = -1;
    int index = 0;
    for (int _lang = QLocale::C; _lang <= QLocale::LastLanguage; ++_lang) {
        QLocale::Language lang = static_cast<QLocale::Language>(_lang);
        const auto locales =
            QLocale::matchingLocales(lang, QLocale::AnyScript, QLocale::AnyTerritory);
        for (auto loc : locales) {
            QString label = QLocale::languageToString(lang);
            auto territory = loc.territory();
            label += QLatin1Char('/');
            label += QLocale::territoryToString(territory);
            if (locale().language() == lang && locale().territory() == territory)
                curLocaleIndex = index;
            localeCombo->addItem(label, loc);
            ++index;
        }
    }
    if (curLocaleIndex != -1)
        localeCombo->setCurrentIndex(curLocaleIndex);
    localeLabel = new QLabel(tr("&Locale"));
    localeLabel->setBuddy(localeCombo);

    firstDayCombo = new QComboBox;
    firstDayCombo->addItem(tr("Sunday"), Qt::Sunday);
    firstDayCombo->addItem(tr("Monday"), Qt::Monday);
    firstDayCombo->addItem(tr("Tuesday"), Qt::Tuesday);
    firstDayCombo->addItem(tr("Wednesday"), Qt::Wednesday);
    firstDayCombo->addItem(tr("Thursday"), Qt::Thursday);
    firstDayCombo->addItem(tr("Friday"), Qt::Friday);
    firstDayCombo->addItem(tr("Saturday"), Qt::Saturday);

    firstDayLabel = new QLabel(tr("Wee&k starts on:"));
    firstDayLabel->setBuddy(firstDayCombo);
//! [10]

    selectionModeCombo = new QComboBox;
    selectionModeCombo->addItem(tr("Single selection"),
                                QCalendarWidget::SingleSelection);
    selectionModeCombo->addItem(tr("None"), QCalendarWidget::NoSelection);

    selectionModeLabel = new QLabel(tr("&Selection mode:"));
    selectionModeLabel->setBuddy(selectionModeCombo);

    gridCheckBox = new QCheckBox(tr("&Grid"));
    gridCheckBox->setChecked(calendar->isGridVisible());

    navigationCheckBox = new QCheckBox(tr("&Navigation bar"));
    navigationCheckBox->setChecked(true);

    horizontalHeaderCombo = new QComboBox;
    horizontalHeaderCombo->addItem(tr("Single letter day names"),
                                   QCalendarWidget::SingleLetterDayNames);
    horizontalHeaderCombo->addItem(tr("Short day names"),
                                   QCalendarWidget::ShortDayNames);
    horizontalHeaderCombo->addItem(tr("None"),
                                   QCalendarWidget::NoHorizontalHeader);
    horizontalHeaderCombo->setCurrentIndex(1);

    horizontalHeaderLabel = new QLabel(tr("&Horizontal header:"));
    horizontalHeaderLabel->setBuddy(horizontalHeaderCombo);

    verticalHeaderCombo = new QComboBox;
    verticalHeaderCombo->addItem(tr("ISO week numbers"),
                                 QCalendarWidget::ISOWeekNumbers);
    verticalHeaderCombo->addItem(tr("None"), QCalendarWidget::NoVerticalHeader);

    verticalHeaderLabel = new QLabel(tr("&Vertical header:"));
    verticalHeaderLabel->setBuddy(verticalHeaderCombo);

//! [11]
    connect(localeCombo, &QComboBox::currentIndexChanged,
            this, &SearchCalender::localeChanged);
    connect(firstDayCombo, &QComboBox::currentIndexChanged,
            this, &SearchCalender::firstDayChanged);
    connect(selectionModeCombo, &QComboBox::currentIndexChanged,
            this, &SearchCalender::selectionModeChanged);
    connect(gridCheckBox, &QCheckBox::toggled,
            calendar, &QCalendarWidget::setGridVisible);
    connect(navigationCheckBox, &QCheckBox::toggled,
            calendar, &QCalendarWidget::setNavigationBarVisible);
    connect(horizontalHeaderCombo, &QComboBox::currentIndexChanged,
            this, &SearchCalender::horizontalHeaderChanged);
    connect(verticalHeaderCombo, &QComboBox::currentIndexChanged,
            this, &SearchCalender::verticalHeaderChanged);
//! [11]

    QHBoxLayout *checkBoxLayout = new QHBoxLayout;
    checkBoxLayout->addWidget(gridCheckBox);
    checkBoxLayout->addStretch();
    checkBoxLayout->addWidget(navigationCheckBox);

    QGridLayout *outerLayout = new QGridLayout;
    outerLayout->addWidget(localeLabel, 0, 0);
    outerLayout->addWidget(localeCombo, 0, 1);
    outerLayout->addWidget(firstDayLabel, 1, 0);
    outerLayout->addWidget(firstDayCombo, 1, 1);
    outerLayout->addWidget(selectionModeLabel, 2, 0);
    outerLayout->addWidget(selectionModeCombo, 2, 1);
    outerLayout->addLayout(checkBoxLayout, 3, 0, 1, 2);
    outerLayout->addWidget(horizontalHeaderLabel, 4, 0);
    outerLayout->addWidget(horizontalHeaderCombo, 4, 1);
    outerLayout->addWidget(verticalHeaderLabel, 5, 0);
    outerLayout->addWidget(verticalHeaderCombo, 5, 1);
    generalOptionsGroupBox->setLayout(outerLayout);

//! [12]
    firstDayChanged(firstDayCombo->currentIndex());
    selectionModeChanged(selectionModeCombo->currentIndex());
    horizontalHeaderChanged(horizontalHeaderCombo->currentIndex());
    verticalHeaderChanged(verticalHeaderCombo->currentIndex());
}

void SearchCalender::createDatesGroupBox()
{
    datesGroupBox = new QGroupBox(tr("Dates"));

    minimumDateEdit = new QDateEdit;
    minimumDateEdit->setDisplayFormat("MMM d yyyy");
    minimumDateEdit->setDateRange(calendar->minimumDate(),
                                  calendar->maximumDate());
    minimumDateEdit->setDate(calendar->minimumDate());

    minimumDateLabel = new QLabel(tr("&Minimum Date:"));
    minimumDateLabel->setBuddy(minimumDateEdit);

    currentDateEdit = new QDateEdit;
    currentDateEdit->setDisplayFormat("MMM d yyyy");
    currentDateEdit->setDate(calendar->selectedDate());
    currentDateEdit->setDateRange(calendar->minimumDate(),
                                  calendar->maximumDate());

    currentDateLabel = new QLabel(tr("&Current Date:"));
    currentDateLabel->setBuddy(currentDateEdit);

    maximumDateEdit = new QDateEdit;
    maximumDateEdit->setDisplayFormat("MMM d yyyy");
    maximumDateEdit->setDateRange(calendar->minimumDate(),
                                  calendar->maximumDate());
    maximumDateEdit->setDate(calendar->maximumDate());

    maximumDateLabel = new QLabel(tr("Ma&ximum Date:"));
    maximumDateLabel->setBuddy(maximumDateEdit);

//! [13] //! [14]
    connect(currentDateEdit, &QDateEdit::dateChanged,
            calendar, &QCalendarWidget::setSelectedDate);
    connect(calendar, &QCalendarWidget::selectionChanged,
            this, &SearchCalender::selectedDateChanged);
    connect(minimumDateEdit, &QDateEdit::dateChanged,
            this, &SearchCalender::minimumDateChanged);
    connect(maximumDateEdit, &QDateEdit::dateChanged,
            this, &SearchCalender::maximumDateChanged);

//! [14]
    QGridLayout *dateBoxLayout = new QGridLayout;
    dateBoxLayout->addWidget(currentDateLabel, 1, 0);
    dateBoxLayout->addWidget(currentDateEdit, 1, 1);
    dateBoxLayout->addWidget(minimumDateLabel, 0, 0);
    dateBoxLayout->addWidget(minimumDateEdit, 0, 1);
    dateBoxLayout->addWidget(maximumDateLabel, 2, 0);
    dateBoxLayout->addWidget(maximumDateEdit, 2, 1);
    dateBoxLayout->setRowStretch(3, 1);

    datesGroupBox->setLayout(dateBoxLayout);
}

void SearchCalender::createTextFormatsGroupBox()
{
    textFormatsGroupBox = new QGroupBox(tr("Text Formats"));

    weekdayColorCombo = createColorComboBox();
    weekdayColorCombo->setCurrentIndex(
            weekdayColorCombo->findText(tr("Black")));

    weekdayColorLabel = new QLabel(tr("&Weekday color:"));
    weekdayColorLabel->setBuddy(weekdayColorCombo);

    weekendColorCombo = createColorComboBox();
    weekendColorCombo->setCurrentIndex(
            weekendColorCombo->findText(tr("Red")));

    weekendColorLabel = new QLabel(tr("Week&end color:"));
    weekendColorLabel->setBuddy(weekendColorCombo);

//! [16] //! [17]
    headerTextFormatCombo = new QComboBox;
    headerTextFormatCombo->addItem(tr("Bold"));
    headerTextFormatCombo->addItem(tr("Italic"));
    headerTextFormatCombo->addItem(tr("Plain"));

    headerTextFormatLabel = new QLabel(tr("&Header text:"));
    headerTextFormatLabel->setBuddy(headerTextFormatCombo);

    firstFridayCheckBox = new QCheckBox(tr("&First Friday in blue"));

    mayFirstCheckBox = new QCheckBox(tr("May &1 in red"));

//! [17] //! [18]
    connect(weekdayColorCombo, &QComboBox::currentIndexChanged,
            this, &SearchCalender::weekdayFormatChanged);
    connect(weekdayColorCombo, &QComboBox::currentIndexChanged,
            this, &SearchCalender::reformatCalendarPage);
    connect(weekendColorCombo, &QComboBox::currentIndexChanged,
            this, &SearchCalender::weekendFormatChanged);
    connect(weekendColorCombo, &QComboBox::currentIndexChanged,
            this, &SearchCalender::reformatCalendarPage);
    connect(headerTextFormatCombo, &QComboBox::currentIndexChanged,
            this, &SearchCalender::reformatHeaders);
    connect(firstFridayCheckBox, &QCheckBox::toggled,
            this, &SearchCalender::reformatCalendarPage);
    connect(mayFirstCheckBox, &QCheckBox::toggled,
            this, &SearchCalender::reformatCalendarPage);

//! [18]
    QHBoxLayout *checkBoxLayout = new QHBoxLayout;
    checkBoxLayout->addWidget(firstFridayCheckBox);
    checkBoxLayout->addStretch();
    checkBoxLayout->addWidget(mayFirstCheckBox);

    QGridLayout *outerLayout = new QGridLayout;
    outerLayout->addWidget(weekdayColorLabel, 0, 0);
    outerLayout->addWidget(weekdayColorCombo, 0, 1);
    outerLayout->addWidget(weekendColorLabel, 1, 0);
    outerLayout->addWidget(weekendColorCombo, 1, 1);
    outerLayout->addWidget(headerTextFormatLabel, 2, 0);
    outerLayout->addWidget(headerTextFormatCombo, 2, 1);
    outerLayout->addLayout(checkBoxLayout, 3, 0, 1, 2);
    textFormatsGroupBox->setLayout(outerLayout);

    weekdayFormatChanged();
    weekendFormatChanged();
//! [19]
    reformatHeaders();
    reformatCalendarPage();
}

QComboBox *SearchCalender::createColorComboBox()
{
    QComboBox *comboBox = new QComboBox;
    comboBox->addItem(tr("Red"), QColor(Qt::red));
    comboBox->addItem(tr("Blue"), QColor(Qt::blue));
    comboBox->addItem(tr("Black"), QColor(Qt::black));
    comboBox->addItem(tr("Magenta"), QColor(Qt::magenta));
    return comboBox;
}
