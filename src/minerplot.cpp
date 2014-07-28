#include <QMetaType>
#include <qapplication.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_scale_draw.h>
#include <qwt_scale_widget.h>
#include <qwt_legend.h>
#include <qwt_legend_label.h>
#include <qwt_plot_canvas.h>
#include "minerplot.h"

#undef SHOW_LEGEND

class TimeScaleDraw: public QwtScaleDraw
{
public:
    TimeScaleDraw(const QTime &base):
        baseTime(base)
    {
    }
    virtual QwtText label(double v) const
    {
        QTime upTime = baseTime.addSecs(static_cast<int>(v));
        return upTime.toString();
    }
private:
    QTime baseTime;
};

static double yMax = 100;

class Background: public QwtPlotItem
{
public:
    Background()
    {
        setZ(0.0);
    }

    virtual int rtti() const
    {
        return QwtPlotItem::Rtti_PlotUserItem;
    }

    virtual void draw(QPainter *painter,
        const QwtScaleMap &, const QwtScaleMap &yMap,
        const QRectF &canvasRect) const
    {
        QColor c(Qt::white);
        QRectF r = canvasRect;

        for (double i = yMax; i > 0; i -= yMax/10)
        {
            r.setBottom(yMap.transform(i - yMax/10));
            r.setTop(yMap.transform(i));
            painter->fillRect(r, c);

            c = c.dark(110);
        }
    }
};

class MinerCurve: public QwtPlotCurve
{
public:
    MinerCurve(const QString &title):
        QwtPlotCurve(title)
    {
        setRenderHint(QwtPlotItem::RenderAntialiased);
    }

    void setColor(const QColor &color)
    {
        QColor c = color;
//        c.setAlpha(150);

        setPen(c, 4);
//        setBrush(c);
    }
};

MinerPlot::MinerPlot(QWidget *parent):
    QwtPlot(parent),
    dataCount(0)
{
    setAutoReplot(false);

    QwtPlotCanvas *canvas = new QwtPlotCanvas();
//    canvas->setBorderRadius(10);
//    canvas->setStyleSheet("QwtPlotCanvas { background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #f6f7fa, stop: 1 #dadbde); }");
    canvas->setStyleSheet("QwtPlotCanvas { background-color: #f6f7fa; }");

    setCanvas(canvas);

    plotLayout()->setAlignCanvasToScales(true);

#ifdef SHOW_LEGEND
    QwtLegend *legend = new QwtLegend;
    legend->setDefaultItemMode(QwtLegendData::Checkable);
    insertLegend(legend, QwtPlot::RightLegend);
#endif

    setAxisTitle(QwtPlot::xBottom, " System Time [h:m:s]");
    setAxisScaleDraw(QwtPlot::xBottom,
        new TimeScaleDraw(minerStat.upTime()));
    setAxisScale(QwtPlot::xBottom, 0, HISTORY);
    setAxisLabelRotation(QwtPlot::xBottom, -50.0);
    setAxisLabelAlignment(QwtPlot::xBottom, Qt::AlignLeft | Qt::AlignBottom);

    /*
     In situations, when there is a label at the most right position of the
     scale, additional space is needed to display the overlapping part
     of the label would be taken by reducing the width of scale and canvas.
     To avoid this "jumping canvas" effect, we add a permanent margin.
     We don't need to do the same for the left border, because there
     is enough space for the overlapping label below the left scale.
     */

    QwtScaleWidget *scaleWidget = axisWidget(QwtPlot::xBottom);
    const int fmh = QFontMetrics(scaleWidget->font()).height();
    scaleWidget->setMinBorderDist(0, fmh / 2);

    setAxisTitle(QwtPlot::yLeft, "Miner Hashrate [GHS]");
    setAxisScale(QwtPlot::yLeft, 0, 100);

#if 0
    Background *bg = new Background();
    bg->attach(this);
#else
    setCanvasBackground(Qt::white);
    QwtPlotGrid *grid = new QwtPlotGrid();
    grid->setPen(QPen(QColor(200, 200, 255, 125)));
    grid->attach(this);
#endif

    MinerCurve *curve;

    curve = new MinerCurve("20s");
    curve->setColor(Qt::yellow);
    curve->attach(this);
    data[value_20s].curve = curve;

    curve = new MinerCurve("av");
    curve->setColor(Qt::red);
    curve->setZ(curve->z() - 1);
    curve->attach(this);
    data[value_av].curve = curve;

    curve = new MinerCurve("cur");
    curve->setColor(Qt::blue);
    curve->setZ(curve->z() - 2);
    curve->attach(this);
    data[value_cur].curve = curve;

    showCurve(data[value_20s].curve, false);
    showCurve(data[value_av].curve, false);
    showCurve(data[value_cur].curve, true);

    for (int i = 0; i < HISTORY; i++)
        timeData[HISTORY - 1 - i] = i;

    timer = startTimer(1000); // 1 second

#ifdef SHOW_LEGEND
    connect(legend, SIGNAL(checked(const QVariant &, bool, int)),
        SLOT(legendChecked(const QVariant &, bool)));
#endif

    connect (this, SIGNAL(addPlotValue(plotInfo)), &minerStat, SLOT(addValue(plotInfo)));
}

void MinerPlot::timerEvent(QTimerEvent *)
{
    int m = 0;

    yMax = minerStat.valueMax;
    if (minerStat.valueMax == 0) {
        yMax = 100;
    } else if (minerStat.valueMax >= 1) {
        while (yMax >= 1) {
            yMax = minerStat.valueMax / qPow(10, m);
            m ++;
        }
        yMax = qPow(10, m-1);
    } else {
        while ((yMax * 10) < 1) {
            m ++;
            yMax = minerStat.valueMax * qPow(10,m);
        }
        yMax = qPow(10, -m);
    }
    setAxisScale(QwtPlot::yLeft, 0, yMax);

    for (int i = 0; i < HISTORY; i ++)
        timeData[HISTORY - 1 - i] = i + minerStat.baseTime->time().secsTo(QTime::currentTime()) - HISTORY;
    setAxisScale(QwtPlot::xBottom, timeData[HISTORY - 1], timeData[0]);

    for (int c = 0; c < value_N; c++) {
        data[c].curve->setRawSamples(minerStat.when, minerStat.value[c], minerStat.valueCount);
    }

    replot();
}

void MinerPlot::legendChecked(const QVariant &itemInfo, bool on)
{
    QwtPlotItem *plotItem = infoToItem(itemInfo);
    if (plotItem)
        showCurve(plotItem, on);
}

void MinerPlot::showCurve(QwtPlotItem *item, bool on)
{
    item->setVisible(on);

#ifdef SHOW_LEGEND
    QwtLegend *lgd = qobject_cast<QwtLegend *>(legend());

    QList<QWidget *> legendWidgets = lgd->legendWidgets(itemToInfo(item));

    if (legendWidgets.size() == 1)
    {
        QwtLegendLabel *legendLabel = qobject_cast<QwtLegendLabel *>(legendWidgets[0]);

        if (legendLabel)
            legendLabel->setChecked(on);
    }
#endif

    replot();
}

void MinerPlot::clearCurve()
{
    minerStat.clearValue();
}
