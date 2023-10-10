#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMouseEvent>
#include <cmath>
#include <QPainter>
#include <qpushbutton.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    mode = SOURCE;
    SourcePoints.clear();
    WindowPoints.clear();
    BufferPoints.clear();

    ui->setupUi(this);
    ui->SourceButton->setStyleSheet("background-color: green; color: white;");
    ui->WindowButton->setStyleSheet("background-color: grey; color: black;");
    connect(ui->SourceButton, &QPushButton::clicked, this, &MainWindow::SwitchToSource);
    connect(ui->WindowButton, &QPushButton::clicked, this, &MainWindow::SwitchToWindow);
    connect(ui->SaveButton, &QPushButton::clicked, this, &MainWindow::WeilerAtherton);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    QPoint p_re = event->pos();
    int x = p_re.x();
    int y = p_re.y();
    Point p(x, y);
    if (event->button() == Qt::LeftButton)
    {
        BufferPoints.push_back(p);
        update();
    }
    else if (event->button() == Qt::RightButton)
    {
        if (BufferPoints.size() >= 3)
        {
            CloseLink();
            update();
        }
    }

}

bool MainWindow::IsClockWise(list<Point> PointLink)
{
    int sum = 0;
    auto current = PointLink.begin();
    auto previous = current;
    current++;
    while (current != PointLink.end())
    {
        sum += (current->x - previous->x) * (current->y + previous->y);
        current++;
        previous++;
    }
    current = PointLink.begin();
    sum += (current->x - previous->x) * (current->y + previous->y);
    return sum < 0;
}

bool MainWindow::JudgeInside(Point p, list<Point> Polygon)
{
    double sum = 0;
    auto current = Polygon.begin();
    auto previous = current;
    current++;
    while (current != Polygon.end())
    {
        Point q(current->x, current->y);
        Point r(previous->x, previous->y);
        double a = p.GetDist(q);
        double b = p.GetDist(r);
        double c = q.GetDist(r);
        double angle = acos((a * a + b * b - c * c) / (2 * a * b));
        sum += angle;
        current++;
        previous++;
    }

    double last_angle = 0;
    current = Polygon.begin();
    {
        Point q(current->x, current->y);
        Point r(previous->x, previous->y);
        double a = p.GetDist(q);
        double b = p.GetDist(r);
        double c = q.GetDist(r);
        double angle = acos((a * a + b * b - c * c) / (2 * a * b));
        sum += angle;
    }
    if (sum - 2 * 3.1415926535 < - 0.01) return 0;
    return 1;
}

void MainWindow::CloseLink()
{
    if (mode == SOURCE)
    {
        Point start = BufferPoints.front();
        int out = -1;
        for (int i = 0; i < SourcePoints.size(); i++)
        {
            bool inside = JudgeInside(start, SourcePoints[i]);
            if (inside)
            {
                out = i;
                break;
            }
        }
        SourcePoints.push_back(BufferPoints);
        if (out == -1) out = SourcePoints.size() - 1;
        OutSource = out;
        for (int i = 0; i < SourcePoints.size(); i++)
        {
            if (i == out)
            {
                if (IsClockWise(SourcePoints[i]))
                {
                    SourcePoints[i].reverse();
                }
            }
            else
            {
                if (!IsClockWise(SourcePoints[i]))
                {
                    SourcePoints[i].reverse();
                }
            }
        }

    }

    else
    {
        Point start = BufferPoints.front();
        int out = -1;
        for (int i = 0; i < WindowPoints.size(); i++)
        {
            bool inside = JudgeInside(start, WindowPoints[i]);
            if (inside)
            {
                out = i;
                break;
            }
        }
        WindowPoints.push_back(BufferPoints);
        if (out == -1) out = WindowPoints.size() - 1;
        OutWindow = out;
        for (int i = 0; i < WindowPoints.size(); i++)
        {
            if (i == out)
            {
                if (IsClockWise(WindowPoints[i]))
                {
                    WindowPoints[i].reverse();
                }
            }
            else
            {
                if (!IsClockWise(WindowPoints[i]))
                {
                    WindowPoints[i].reverse();
                }
            }
        }

    }
    BufferPoints.clear();
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter paint(this);
    paint.setPen(Qt::red);
    for (int i = 0; i < SourcePoints.size(); i++)
    {
        auto current = SourcePoints[i].begin();
        auto previous = current;
        current++;
        while (current != SourcePoints[i].end())
        {
            paint.drawLine(current->x, current->y, previous->x, previous->y);
            current++;
            previous++;
        }
        current = SourcePoints[i].begin();;
        paint.drawLine(current->x, current->y, previous->x, previous->y);
    }
    paint.setPen(Qt::blue);
    for (int i = 0; i < WindowPoints.size(); i++)
    {
        auto current = WindowPoints[i].begin();
        auto previous = current;
        current++;
        while (current != WindowPoints[i].end())
        {
            paint.drawLine(current->x, current->y, previous->x, previous->y);
            current++;
            previous++;
        }
        current = WindowPoints[i].begin();;
        paint.drawLine(current->x, current->y, previous->x, previous->y);
    }

    if(mode == SOURCE) 	paint.setPen(Qt::red);
    else paint.setPen(Qt::blue);
    if (BufferPoints.size() >= 2)
    {
        auto current = BufferPoints.begin();
        auto previous = current;
        current++;
        while (current != BufferPoints.end())
        {
            paint.drawLine(current->x, current->y, previous->x, previous->y);
            current++;
            previous++;
        }
    }

    paint.setPen(Qt::black);
    for (int i = 0; i < Result.size(); i++)
    {
        if (Result[i].size() > 2)
        {
            int current = 1;
            int previous = 0;
            while (current < Result[i].size())
            {
                paint.drawLine(Result[i][current].x, Result[i][current].y, Result[i][previous].x, Result[i][previous].y);
                current++;
                previous++;
            }
            current = 0;
            paint.drawLine(Result[i][current].x, Result[i][current].y, Result[i][previous].x, Result[i][previous].y);
        }
    }
}

void MainWindow::SwitchToSource()
{
    mode = SOURCE;
    ui->SourceButton->setStyleSheet("background-color: green; color: white;");
    ui->WindowButton->setStyleSheet("background-color: grey; color: black;");
    BufferPoints.clear();
}
void MainWindow::SwitchToWindow()
{
    mode = WINDOW;
    ui->WindowButton->setStyleSheet("background-color: green; color: white;");
    ui->SourceButton->setStyleSheet("background-color: grey; color: black;");
    BufferPoints.clear();
}

Point MainWindow::GetIntersect(Point pStart, Point pEnd, Point qStart, Point qEnd)
{
    int aP = pEnd.y - pStart.y;
    int bP = pStart.x - pEnd.x;
    int cP = aP * pStart.x + bP * pStart.y;

    int aQ = qEnd.y - qStart.y;
    int bQ = qStart.x - qEnd.x;
    int cQ = aQ * qStart.x + bQ * qStart.y;

    int det = aP * bQ - aQ * bP;
    if (det == 0)
    {
        return Point(-1, -1);
    }
    int x = (bQ * cP - bP * cQ) / det;
    int y = (aP * cQ - aQ * cP) / det;
    if (x > max(pEnd.x, pStart.x) || x < min(pEnd.x, pStart.x) || x > max(qEnd.x, qStart.x) || x < min(qEnd.x, qStart.x))
        return Point(-1, -1);
    if (y > max(pEnd.y, pStart.y) || y < min(pEnd.y, pStart.y) || y > max(qEnd.y, qStart.y) || y < min(qEnd.y, qStart.y))
        return Point(-1, -1);
    Point theP(x, y);
    if (det > 0)
        theP.enter = true;
    return theP;
}

double MainWindow::GetK(Point start, Point end, Point middle)
{

    double t = sqrt((end.x - start.x) * (end.x - start.x) + (end.y - start.y) * (end.y - start.y));
    double d = sqrt((middle.x - start.x) * (middle.x - start.x) + (middle.y - start.y) * (middle.y - start.y));
    return d / t;
}

void MainWindow::GetIntersections(Point startA, Point endA, int nPolygon, int nLine, vector<list<Point>>& window,
                                  vector<vector<vector<Intersection>>>& listA, vector<vector<vector<Intersection>>>& listB)
{

    for (int i = 0; i < window.size(); i++)
    {
        list<Point> windowList = window[i];
        auto current = windowList.begin();
        auto previous = current;
        int currentNum = 0;
        current++;
        while (current != windowList.end())
        {
            Point startB = Point(previous->x, previous->y);
            Point endB = Point(current->x, current->y);
            Point intersect = GetIntersect(startA, endA, startB, endB);
            if (intersect.x >= 0 && intersect.y >= 0)
            {
                double k = GetK(startA, endA, intersect);
                double kk = GetK(startB, endB, intersect);
                Intersection a = Intersection(intersect.x, intersect.y, intersect.enter, k, SOURCE, i, currentNum);
                Intersection b = Intersection(intersect.x, intersect.y, intersect.enter, kk, WINDOW, nPolygon, nLine);
                listA[nPolygon][nLine].push_back(a);
                listB[i][currentNum].push_back(b);
            }
            current++;
            previous++;
            currentNum++;
        }
        current = WindowPoints[i].begin();
        {
            Point startB = Point(previous->x, previous->y);
            Point endB = Point(current->x, current->y);
            Point intersect = GetIntersect(startA, endA, startB, endB);
            if (intersect.x >= 0 && intersect.y >= 0)
            {
                double k = GetK(startA, endA, intersect);
                double kk = GetK(startB, endB, intersect);
                Intersection a = Intersection(intersect.x, intersect.y, intersect.enter, k, SOURCE, i, currentNum);
                Intersection b = Intersection(intersect.x, intersect.y, intersect.enter, kk, WINDOW, nPolygon, nLine);
                listA[nPolygon][nLine].push_back(a);
                listB[i][currentNum].push_back(b);
            }
        }
    }
}

void MainWindow::SortIntersections(vector<Intersection>& tList)
{
    if (tList.size() <= 1) return;
    for (int i = 0; i < tList.size() - 1; i++)
    {
        for (int j = i + 1; j < tList.size(); j++)
        {
            if (tList[i].k > tList[j].k)
            {
                Intersection temp = tList[i];
                tList[i] = tList[j];
                tList[j] = temp;
            }
        }
    }
}

void MainWindow::FindRelatedPlace(bool type, Intersection p, int& npolygon, int& nline, int& npoint)
{
    nline = p.lineNumber;
    npolygon = p.polygonNumber;
    if (type == SOURCE)
    {
        for (int i = 0; i < listWindow[npolygon][nline].size(); i++)
        {
            if (listWindow[npolygon][nline][i].xCoordinate == p.xCoordinate && listWindow[npolygon][nline][i].yCoordinate == p.yCoordinate)
            {
                npoint = i;
                return;
            }
        }
    }
    else
    {
        for (int i = 0; i < listSource[npolygon][nline].size(); i++)
        {
            if (listSource[npolygon][nline][i].xCoordinate == p.xCoordinate && listSource[npolygon][nline][i].yCoordinate == p.yCoordinate)
            {
                npoint = i;
                return;
            }
        }
    }
}

void MainWindow::FindNextPlace(int& npolygon, int& nline, int& npoint, bool& pmode,
                               int cpolygon, int cline, int cpoint, bool cmode, int current)
{
    if (cpolygon == -1 || cline == -1 || cpoint == -1)
    {
        for (int i = 0; i < visit.size(); i++)
        {
            for (int j = 0; j < visit[i].size(); j++)
            {
                for (int k = 0; k < visit[i][j].size(); k++)
                {
                    if (visit[i][j][k] == 0)
                    {
                        npolygon = i;
                        nline = j;
                        npoint = k;
                        pmode = 1;
                        Intersection the_point = listSource[i][j][k];
                        if (!the_point.enter)
                        {
                            pmode = WINDOW;
                            FindRelatedPlace(SOURCE, the_point, npolygon, nline, npoint);
                            return;
                        }
                        return;
                    }
                }
            }
        }
        npolygon = -1;
        nline = -1;
        npoint = -1;
        return;
    }
    else
    {
        int length = 0;
        list<Point> tlist;
        if (cmode == SOURCE)
        {
            length = listSource[cpolygon][cline].size();
            while (cpoint + 1 >= length)
            {
                cline++;
                if (cline >= SourcePoints[cpolygon].size()) cline = 0;
                cpoint = -1;
                length = listSource[cpolygon][cline].size();

                auto t_iterator = SourcePoints[cpolygon].begin();
                for (int i = 0; i < cline; i++)
                {
                    t_iterator++;
                }
                Point the_vertex = *t_iterator;
                Result[current].push_back(the_vertex);
            }
            cpoint++;
            Intersection the_point = listSource[cpolygon][cline][cpoint];
            if (!the_point.enter)
            {
                pmode = WINDOW;
                FindRelatedPlace(SOURCE, the_point, npolygon, nline, npoint);
                return;
            }
            else
            {
                pmode = SOURCE;
                npolygon = cpolygon;
                nline = cline;
                npoint = cpoint;
                return;
            }
        }
        else
        {
            length = listWindow[cpolygon][cline].size();
            while (cpoint + 1 >= length)
            {
                cline++;
                if (cline >= WindowPoints[cpolygon].size()) cline = 0;
                cpoint = -1;
                length = listWindow[cpolygon][cline].size();

                auto t_iterator = WindowPoints[cpolygon].begin();
                for (int i = 0; i < cline; i++)
                {
                    t_iterator++;
                }
                Point the_vertex = *t_iterator;
                Result[current].push_back(the_vertex);
            }
            cpoint++;
            Intersection the_point = listWindow[cpolygon][cline][cpoint];
            if (the_point.enter)
            {
                pmode = SOURCE;
                FindRelatedPlace(WINDOW, the_point, npolygon, nline, npoint);
                return;
            }
            else
            {
                pmode = WINDOW;
                npolygon = cpolygon;
                nline = cline;
                npoint = cpoint;
                return;
            }
        }
    }
}

void MainWindow::WeilerAtherton()
{

    listSource.clear();
    visit.clear();
    for (int i = 0; i < SourcePoints.size(); i++)
    {
        vector<vector<Intersection>> empty_one;
        vector<vector<bool>> empty_boolean;
        empty_one.clear();
        empty_boolean.clear();
        listSource.push_back(empty_one);
        visit.push_back(empty_boolean);
        for (int j = 0; j < SourcePoints[i].size(); j++)
        {
            vector<Intersection> new_empty;
            vector<bool> new_bool;
            new_empty.clear();
            new_bool.clear();
            listSource[i].push_back(new_empty);
            visit[i].push_back(new_bool);
        }
    }
    listWindow.clear();
    for (int i = 0; i < WindowPoints.size(); i++)
    {
        vector<vector<Intersection>> empty_one;
        empty_one.clear();
        listWindow.push_back(empty_one);
        for (int j = 0; j < WindowPoints[i].size(); j++)
        {
            vector<Intersection> new_empty;
            new_empty.clear();
            listWindow[i].push_back(new_empty);
        }
    }


    for (int ii = 0; ii < SourcePoints.size(); ii++)
    {
        list<Point> source_list = SourcePoints[ii];
        vector<list<Point>> window_lists = WindowPoints;
        auto current = source_list.begin();
        auto previous = current;
        current++;
        int current_num = 0;
        while (current != source_list.end())
        {
            Point start = Point(previous->x, previous->y);
            Point end = Point(current->x, current->y);
            GetIntersections(start, end, ii, current_num, window_lists, listSource, listWindow);


            current++;
            previous++;
            current_num++;
        }
        current = source_list.begin();
        {
            Point start = Point(previous->x, previous->y);
            Point end = Point(current->x, current->y);
            GetIntersections(start, end, ii, current_num, window_lists, listSource, listWindow);
        }
    }
    for (int i = 0; i < listSource.size(); i++)
    {
        for (int j = 0; j < listSource[i].size(); j++)
        {
            SortIntersections(listSource[i][j]);
            for (int k = 0; k < listSource[i][j].size(); k++)
            {
                visit[i][j].push_back(0);
            }
        }
    }
    for (int i = 0; i < listWindow.size(); i++)
    {
        for (int j = 0; j < listWindow[i].size(); j++)
        {
            SortIntersections(listWindow[i][j]);
        }
    }

    Result.clear();
    vector<Point> empty_kebab;
    empty_kebab.clear();
    Result.push_back(empty_kebab);
    int current = 0;
    int p_polygon = -1;
    int p_line = -1;
    int p_point = -1;
    bool p_mode = SOURCE;
    while(1)
    {
        int n_polygon = -1;
        int n_line = -1;
        int n_point = -1;
        bool n_mode = 0;
        int v_polygon = -1;
        int v_line = -1;
        int v_point = -1;
        FindNextPlace(n_polygon, n_line, n_point, n_mode, p_polygon, p_line, p_point, p_mode, current);
        if (n_polygon == -1 || n_line == -1 || n_point == -1)
        {
            break;
        }
        if (n_mode == WINDOW)
        {
            FindRelatedPlace(WINDOW, listWindow[n_polygon][n_line][n_point], v_polygon, v_line, v_point);
        }
        else
        {
            v_polygon = n_polygon; v_line = n_line; v_point = n_point;
        }
        if (visit[v_polygon][v_line][v_point] == 1)
        {
            vector<Point> empty_k;
            empty_k.clear();
            Result.push_back(empty_k);
            current++;
            p_polygon = -1;
            p_line = -1;
            p_point = -1;
            p_mode = SOURCE;
        }
        else
        {
            Point nova = Point(listSource[v_polygon][v_line][v_point].xCoordinate, listSource[v_polygon][v_line][v_point].yCoordinate);
            Result[current].push_back(nova);
            visit[v_polygon][v_line][v_point] = 1;
            p_polygon = n_polygon;
            p_line = n_line;
            p_point = n_point;
            p_mode = n_mode;
        }
    }
    JudgeSpecial();
    repaint();
}

bool MainWindow::HasNoIntersection(bool mode, int num)
{
    if (mode == SOURCE)
    {
        for (int i = 0; i < listSource[num].size(); i++)
        {
            if (listSource[num][i].size() != 0) return 0;
        }
    }
    else
    {
        for (int i = 0; i < listWindow[num].size(); i++)
        {
            if (listWindow[num][i].size() != 0) return 0;
        }
    }
    return 1;
}

bool MainWindow::JudgeAdd(bool mode, int num)
{
    if (mode == SOURCE)
    {
        for (int i = 0; i < WindowPoints.size(); i++)
        {
            auto current = SourcePoints[num].begin();
            while (current != SourcePoints[num].end())
            {
                Point c_point = Point(current->x, current->y);
                if (i != OutWindow)
                {
                    if (JudgeInside(c_point, WindowPoints[i])) return 0;
                }
                else
                {
                    if (!JudgeInside(c_point, WindowPoints[i])) return 0;
                }
                current++;
            }
        }
    }
    else
    {
        for (int i = 0; i < SourcePoints.size(); i++)
        {
            auto current = WindowPoints[num].begin();
            while (current != WindowPoints[num].end())
            {
                Point c_point = Point(current->x, current->y);
                if (i != OutSource)
                {
                    if (JudgeInside(c_point, SourcePoints[i])) return 0;
                }
                else
                {

                    if (!JudgeInside(c_point, SourcePoints[i])) return 0;
                }
                current++;
            }
        }
    }
    return 1;
}

void MainWindow::JudgeSpecial()
{
    for (int i = 0; i < SourcePoints.size(); i++)
    {
        if (HasNoIntersection(SOURCE, i))
        {
            if (JudgeAdd(SOURCE, i))
            {
                vector<Point> nova_list;
                nova_list.clear();
                Result.push_back(nova_list);
                auto current = SourcePoints[i].begin();
                while (current != SourcePoints[i].end())
                {
                    Point c_point = Point(current->x, current->y);
                    Result[Result.size() - 1].push_back(c_point);
                    current++;
                }
            }
        }
    }

    for (int i = 0; i < WindowPoints.size(); i++)
    {
        if (HasNoIntersection(WINDOW, i))
        {
            if (JudgeAdd(WINDOW, i))
            {
                vector<Point> nova_list;
                nova_list.clear();
                Result.push_back(nova_list);
                auto current = WindowPoints[i].begin();
                while (current != WindowPoints[i].end())
                {
                    Point c_point = Point(current->x, current->y);
                    Result[Result.size() - 1].push_back(c_point);
                    current++;
                }
            }
        }
    }
}
