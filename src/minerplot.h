#include <qwt_plot.h>
#include <qwt_legend.h>
#include <qwt_plot_canvas.h>
#include "minerstat.h"

class QwtPlotCurve;

class MinerPlot : public QwtPlot
{
    Q_OBJECT
public:
    MinerPlot(QWidget * = 0);
    const QwtPlotCurve *minerCurve(int id) const
    {
        return data[id].curve;
    }
    void clearCurve();
//    void restart();

signals:
    void addPlotValue(const plotInfo &);

protected:
    void timerEvent(QTimerEvent *e);

private Q_SLOTS:
    void legendChecked(const QVariant &, bool on);

private:
    void showCurve(QwtPlotItem *, bool on);
    void plotReconfig(QwtPlotCanvas *, QwtLegend *);

    struct
    {
        QwtPlotCurve *curve;
        double data[HISTORY];
    } data[value_N];

    double timeData[HISTORY];

    int dataCount;
    MinerStat minerStat;
    int timer;

    QwtPlotCanvas *canvas;
    QwtLegend *mylegend;
};
