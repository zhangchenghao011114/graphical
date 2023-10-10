#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMouseEvent>
#include <vector>
#include <list>
using namespace std;
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

#define SOURCE 1
#define WINDOW 0

struct Point
{
    int x;
    int y;
    bool enter;

    Point(int tx, int ty)
    {
        x = tx;
        y = ty;
        enter = false;
    }

    double GetDist(Point a)
    {
        int dx = a.x - x;
        int dy = a.y - y;
        return sqrt(dx * dx + dy * dy);
    }
};

struct Intersection
{
    int xCoordinate;
    int yCoordinate;
    bool enter;
    double k;
    bool type;
    int polygonNumber;
    int lineNumber;
    Intersection(int tx, int ty, bool tenter, double tk, bool ttype, int npolygon, int nline)
    {
        xCoordinate = tx;
        yCoordinate = ty;
        enter = tenter;
        k = tk;
        type = ttype;
        polygonNumber = npolygon;
        lineNumber = nline;
    }
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
protected:	//mouse
    void mousePressEvent(QMouseEvent *event);
    vector<list<Point>> SourcePoints;
    int OutSource = -1;
    vector<list<Point>> WindowPoints;
    int OutWindow = -1;
    list<Point> BufferPoints;

    bool mode = 1;
    vector<vector<vector<Intersection>>> listSource;
    vector<vector<vector<bool>>> visit;
    vector<vector<vector<Intersection>>> listWindow;
    vector<vector<Point>> Result;

    Point GetIntersect(Point pStart, Point pEnd, Point qStart, Point qEnd);

    double GetK(Point start, Point end, Point middle);

    void GetIntersections(Point startA, Point endA, int npolygon, int nline, vector<list<Point>>& Window,
                          vector<vector<vector<Intersection>>>& listA, vector<vector<vector<Intersection>>>& listB);

    void SortIntersections(vector<Intersection>& tList);

    void FindRelatedPlace(bool type, Intersection p, int& npolygon, int& nline, int& npoint);

    void FindNextPlace(int& npolygon, int& nline, int& npoint, bool& pmode,
                       int cpolygon, int cline, int cpoint, bool cmode, int current);
    void CloseLink();

    bool IsClockWise(list<Point> PointLink);

    bool JudgeInside(Point p, list<Point> Polygon);

    void paintEvent(QPaintEvent *event);

    void JudgeSpecial();

    bool HasNoIntersection(bool mode, int num);

    bool JudgeAdd(bool mode, int num);

private slots:
    void SwitchToSource();
    void SwitchToWindow();
    void WeilerAtherton();
private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
