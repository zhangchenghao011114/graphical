#pragma once

#include <QtWidgets/QMainWindow>
#include <QMouseEvent>
#include <vector>
#include <list>
using namespace std;

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
        enter = 0;
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
    int x_coordinate;
    int y_coordinate;
    bool enter;
    double k;
    bool type;
    int polygon_number; //��λ
    int lineNumber;   //��λ
    Intersection(int tx, int ty, bool tenter, double tk, bool ttype, int npolygon, int nline)
    {
        x_coordinate = tx;
        y_coordinate = ty;
        enter = tenter;
        k = tk;
        type = ttype;
        polygon_number = npolygon;
        lineNumber = nline;
    }
};

class Cropping : public QMainWindow
{
    Q_OBJECT

public:
    Cropping(QWidget *parent = Q_NULLPTR);

protected:	//mouse
    void mousePressEvent(QMouseEvent *event);        //����
    vector<list<Point>> SourcePoints;
    int OutSource = -1;
    vector<list<Point>> WindowPoints;
    int OutWindow = -1;
    list<Point> BufferPoints;
    //1:Ĭ�ϴ��ü�����Σ�0�����ڶ����
    bool mode = 1;
    vector<vector<vector<Intersection>>> list_source;
    vector<vector<vector<bool>>> visit;
    vector<vector<vector<Intersection>>> list_window;
    vector<vector<Point>> Result;
    //��
    Point GetIntersect(Point p_start, Point p_end, Point q_start, Point q_end);
    //�󽻵�k
    double GetK(Point start, Point end, Point middle);
    //�����н���
    void GetIntersections(Point start_a, Point end_a, int npolygon, int nline, vector<list<Point>>& Window,
                          vector<vector<vector<Intersection>>>& list_a, vector<vector<vector<Intersection>>>& list_b);	//�պ�ͼ��
    //���򽻵�
    void SortIntersections(vector<Intersection>& t_list);
    //�Ҷ�Ӧ��λ��
    void FindRelatedPlace(bool type, Intersection p, int& npolygon, int& nline, int& npoint);
    //�ҿ�ʼλ��
    void FindNextPlace(int& npolygon, int& nline, int& npoint, bool& pmode,
                       int cpolygon, int cline, int cpoint, bool cmode, int current);
    void CloseLink();
    //�ж�˳ʱ��
    bool IsClockWise(list<Point> PointLink);
    //�жϵ��ڶ������
    bool JudgeInside(Point p, list<Point> Polygon);
    //����
    void paintEvent(QPaintEvent *event);
    //����û�н����ͼ��
    void JudgeSpecial();
    //�ж���û�н���
    bool HasNoIntersection(bool mode, int num);
    //�ж��Ƿ������������
    bool JudgeAdd(bool mode, int num);
    //�л�ģʽ
private slots:
    void SwitchToSource();
    void SwitchToWindow();
    //������
    void Weiler_Atherton();
private:
    Ui::CroppingClass ui;
};
