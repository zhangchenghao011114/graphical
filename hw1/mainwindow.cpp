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
    ResetAll();
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
    QPoint point = event->pos();
    int x = point.x();
    int y = point.y();
    Point p(x, y);

    if (event->button() == Qt::LeftButton)
    {
        handleLeftMouseClick(p);
    }
    else if (event->button() == Qt::RightButton)
    {
        handleRightMouseClick();
    }
}

void MainWindow::handleLeftMouseClick(const Point& p)
{
    BufferPoints.push_back(p);
    update();
}

void MainWindow::handleRightMouseClick()
{
    if (BufferPoints.size() >= 3)
    {
        CloseLink();
        update();
    }
}

bool MainWindow::IsClockWise(list<Point> pointLink)
{
    int sum = 0;
    auto current = pointLink.begin();
    auto previous = current;
    current++;
    while (current != pointLink.end())
    {
        sum += (current->x - previous->x) * (current->y + previous->y);
        current++;
        previous++;
    }
    sum += (pointLink.begin()->x - previous->x) * (pointLink.begin()->y + previous->y);
    return sum < 0;
}

bool MainWindow::JudgeInside(Point p, list<Point> Polygon)
{
    double angleSum = 0;
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
        angleSum += angle;
        current++;
        previous++;
    }

    current = Polygon.begin();
    Point q(current->x, current->y);
    Point r(previous->x, previous->y);
    double a = p.GetDist(q);
    double b = p.GetDist(r);
    double c = q.GetDist(r);
    double angle = acos((a * a + b * b - c * c) / (2 * a * b));
    angleSum += angle;

    if (angleSum - 2 * 3.1415926535 < -0.01) {
        return false;
    }
    return true;
}
void MainWindow::CloseLink()
{
    Point start = BufferPoints.front();
    int out = -1;
    vector<list<Point>>* pointsList;
    int* outIndex;

    if (mode == SOURCE)
    {
        pointsList = &SourcePoints;
        outIndex = &OutSource;
    }
    else
    {
        pointsList = &WindowPoints;
        outIndex = &OutWindow;
    }

    for (int i = 0; i < pointsList->size(); i++)
    {
        bool inside = JudgeInside(start, (*pointsList)[i]);
        if (inside)
        {
            out = i;
            break;
        }
    }

    pointsList->push_back(BufferPoints);

    if (out == -1)
        out = pointsList->size() - 1;

    *outIndex = out;

    for (int i = 0; i < pointsList->size(); i++)
    {
        if (i == out)
        {
            if (IsClockWise((*pointsList)[i]))
            {
                (*pointsList)[i].reverse();
            }
        }
        else
        {
            if (!IsClockWise((*pointsList)[i]))
            {
                (*pointsList)[i].reverse();
            }
        }
    }

    BufferPoints.clear();
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    // 绘制SourcePoints
    painter.setPen(Qt::red);
    for (int i = 0; i < SourcePoints.size(); i++)
    {
        auto currentPoint = SourcePoints[i].begin();
        auto previousPoint = currentPoint;
        currentPoint++;
        while (currentPoint != SourcePoints[i].end())
        {
            painter.drawLine(currentPoint->x, currentPoint->y, previousPoint->x, previousPoint->y);
            currentPoint++;
            previousPoint++;
        }
        currentPoint = SourcePoints[i].begin();;
        painter.drawLine(currentPoint->x, currentPoint->y, previousPoint->x, previousPoint->y);
    }

    // 绘制WindowPoints
    painter.setPen(Qt::blue);
    for (int i = 0; i < WindowPoints.size(); i++)
    {
        auto currentPoint = WindowPoints[i].begin();
        auto previousPoint = currentPoint;
        currentPoint++;
        while (currentPoint != WindowPoints[i].end())
        {
            painter.drawLine(currentPoint->x, currentPoint->y, previousPoint->x, previousPoint->y);
            currentPoint++;
            previousPoint++;
        }
        currentPoint = WindowPoints[i].begin();;
        painter.drawLine(currentPoint->x, currentPoint->y, previousPoint->x, previousPoint->y);
    }

    // 绘制BufferPoints
    if(mode == SOURCE)
        painter.setPen(Qt::red);
    else
        painter.setPen(Qt::blue);
    if (BufferPoints.size() >= 2)
    {
        auto currentPoint = BufferPoints.begin();
        auto previousPoint = currentPoint;
        currentPoint++;
        while (currentPoint != BufferPoints.end())
        {
            painter.drawLine(currentPoint->x, currentPoint->y, previousPoint->x, previousPoint->y);
            currentPoint++;
            previousPoint++;
        }
    }

    // 绘制Result
    painter.setPen(Qt::black);
    for (int i = 0; i < Result.size(); i++)
    {
        if (Result[i].size() > 2)
        {
            int current = 1;
            int previous = 0;
            while (current < Result[i].size())
            {
                painter.drawLine(Result[i][current].x, Result[i][current].y, Result[i][previous].x, Result[i][previous].y);
                current++;
                previous++;
            }
            current = 0;
            painter.drawLine(Result[i][current].x, Result[i][current].y, Result[i][previous].x, Result[i][previous].y);
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

    int determinant = aP * bQ - aQ * bP;
    if (determinant == 0)
    {
        return Point(-1, -1);
    }
    int x = (bQ * cP - bP * cQ) / determinant;
    int y = (aP * cQ - aQ * cP) / determinant;
    if (x > max(pEnd.x, pStart.x) || x < min(pEnd.x, pStart.x) || x > max(qEnd.x, qStart.x) || x < min(qEnd.x, qStart.x))
        return Point(-1, -1);
    if (y > max(pEnd.y, pStart.y) || y < min(pEnd.y, pStart.y) || y > max(qEnd.y, qStart.y) || y < min(qEnd.y, qStart.y))
        return Point(-1, -1);
    Point intersectionPoint(x, y);
    if (determinant > 0)
        intersectionPoint.enter = true;
    return intersectionPoint;
}

double MainWindow::GetK(Point start, Point end, Point middle)
{
    double distanceSE = sqrt((end.x - start.x) * (end.x - start.x) + (end.y - start.y) * (end.y - start.y));
    double distanceSM = sqrt((middle.x - start.x) * (middle.x - start.x) + (middle.y - start.y) * (middle.y - start.y));

    return distanceSM / distanceSE;
}

void MainWindow::GetIntersections(Point startA, Point endA, int nPolygon, int nLine, vector<list<Point>>& window,
                                  vector<vector<vector<Intersection>>>& listA, vector<vector<vector<Intersection>>>& listB)
{
    for (int i = 0; i < window.size(); i++)
    {
        list<Point> windowList = window[i];
        auto currentPoint = windowList.begin();
        auto previousPoint = currentPoint;
        int currentNum = 0;
        currentPoint++;
        while (currentPoint != windowList.end())
        {
            Point startB = Point(previousPoint->x, previousPoint->y);
            Point endB = Point(currentPoint->x, currentPoint->y);
            Point intersection = GetIntersect(startA, endA, startB, endB);
            if (intersection.x >= 0 && intersection.y >= 0)
            {
                double k = GetK(startA, endA, intersection);
                double kk = GetK(startB, endB, intersection);
                Intersection a = Intersection(intersection.x, intersection.y, intersection.enter, k, SOURCE, i, currentNum);
                Intersection b = Intersection(intersection.x, intersection.y, intersection.enter, kk, WINDOW, nPolygon, nLine);
                listA[nPolygon][nLine].push_back(a);
                listB[i][currentNum].push_back(b);
            }
            currentPoint++;
            previousPoint++;
            currentNum++;
        }

        currentPoint = windowList.begin();
        {
            Point startB = Point(previousPoint->x, previousPoint->y);
            Point endB = Point(currentPoint->x, currentPoint->y);
            Point intersection = GetIntersect(startA, endA, startB, endB);
            if (intersection.x >= 0 && intersection.y >= 0)
            {
                double k = GetK(startA, endA, intersection);
                double kk = GetK(startB, endB, intersection);
                Intersection a = Intersection(intersection.x, intersection.y, intersection.enter, k, SOURCE, i, currentNum);
                Intersection b = Intersection(intersection.x, intersection.y, intersection.enter, kk, WINDOW, nPolygon, nLine);
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

//void MainWindow::FindRelatedPlace(bool type, Intersection p, int& npolygon, int& nline, int& npoint)
//{
//    nline = p.lineNumber;
//    npolygon = p.polygonNumber;
//    if (type == SOURCE)
//    {
//        for (int i = 0; i < listWindow[npolygon][nline].size(); i++)
//        {
//            if (listWindow[npolygon][nline][i].xCoordinate == p.xCoordinate && listWindow[npolygon][nline][i].yCoordinate == p.yCoordinate)
//            {
//                npoint = i;
//                return;
//            }
//        }
//    }
//    else
//    {
//        for (int i = 0; i < listSource[npolygon][nline].size(); i++)
//        {
//            if (listSource[npolygon][nline][i].xCoordinate == p.xCoordinate && listSource[npolygon][nline][i].yCoordinate == p.yCoordinate)
//            {
//                npoint = i;
//                return;
//            }
//        }
//    }
//}
void MainWindow::FindRelatedPlace(bool type, Intersection targetIntersection, int& targetPolygon, int& targetLine, int& targetPoint)
{
    targetLine = targetIntersection.lineNumber;
    targetPolygon = targetIntersection.polygonNumber;

    int numPointsToSearch = (type == SOURCE) ? listWindow[targetPolygon][targetLine].size() : listSource[targetPolygon][targetLine].size();

    for (int i = 0; i < numPointsToSearch; i++)
    {
        Intersection& currentPoint = (type == SOURCE) ? listWindow[targetPolygon][targetLine][i] : listSource[targetPolygon][targetLine][i];

        if (currentPoint.xCoordinate == targetIntersection.xCoordinate && currentPoint.yCoordinate == targetIntersection.yCoordinate)
        {
            targetPoint = i;
            return;
        }
    }
}


//void MainWindow::FindNextPlace(int& npolygon, int& nline, int& npoint, bool& pmode,
//                               int cpolygon, int cline, int cpoint, bool cmode, int current)
//{
//    if (cpolygon == -1 || cline == -1 || cpoint == -1)
//    {
//        for (int i = 0; i < visit.size(); i++)
//        {
//            for (int j = 0; j < visit[i].size(); j++)
//            {
//                for (int k = 0; k < visit[i][j].size(); k++)
//                {
//                    if (visit[i][j][k] == 0)
//                    {
//                        npolygon = i;
//                        nline = j;
//                        npoint = k;
//                        pmode = 1;
//                        Intersection the_point = listSource[i][j][k];
//                        if (!the_point.enter)
//                        {
//                            pmode = WINDOW;
//                            FindRelatedPlace(SOURCE, the_point, npolygon, nline, npoint);
//                            return;
//                        }
//                        return;
//                    }
//                }
//            }
//        }
//        npolygon = -1;
//        nline = -1;
//        npoint = -1;
//        return;
//    }
//    else
//    {
//        int length = 0;
//        list<Point> tlist;
//        if (cmode == SOURCE)
//        {
//            length = listSource[cpolygon][cline].size();
//            while (cpoint + 1 >= length)
//            {
//                cline++;
//                if (cline >= SourcePoints[cpolygon].size()) cline = 0;
//                cpoint = -1;
//                length = listSource[cpolygon][cline].size();

//                auto t_iterator = SourcePoints[cpolygon].begin();
//                for (int i = 0; i < cline; i++)
//                {
//                    t_iterator++;
//                }
//                Point the_vertex = *t_iterator;
//                Result[current].push_back(the_vertex);
//            }
//            cpoint++;
//            Intersection the_point = listSource[cpolygon][cline][cpoint];
//            if (!the_point.enter)
//            {
//                pmode = WINDOW;
//                FindRelatedPlace(SOURCE, the_point, npolygon, nline, npoint);
//                return;
//            }
//            else
//            {
//                pmode = SOURCE;
//                npolygon = cpolygon;
//                nline = cline;
//                npoint = cpoint;
//                return;
//            }
//        }
//        else
//        {
//            length = listWindow[cpolygon][cline].size();
//            while (cpoint + 1 >= length)
//            {
//                cline++;
//                if (cline >= WindowPoints[cpolygon].size()) cline = 0;
//                cpoint = -1;
//                length = listWindow[cpolygon][cline].size();

//                auto t_iterator = WindowPoints[cpolygon].begin();
//                for (int i = 0; i < cline; i++)
//                {
//                    t_iterator++;
//                }
//                Point the_vertex = *t_iterator;
//                Result[current].push_back(the_vertex);
//            }
//            cpoint++;
//            Intersection the_point = listWindow[cpolygon][cline][cpoint];
//            if (the_point.enter)
//            {
//                pmode = SOURCE;
//                FindRelatedPlace(WINDOW, the_point, npolygon, nline, npoint);
//                return;
//            }
//            else
//            {
//                pmode = WINDOW;
//                npolygon = cpolygon;
//                nline = cline;
//                npoint = cpoint;
//                return;
//            }
//        }
//    }
//}

void MainWindow::FindNextPlace(int& npolygon, int& nline, int& npoint, bool& pmode,
                               int cpolygon, int cline, int cpoint, bool cmode, int current)
{
    if (cpolygon == -1 || cline == -1 || cpoint == -1)
    {
        // Find the next unvisited place
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
        // If no unvisited place found, set the output parameters to -1
        npolygon = -1;
        nline = -1;
        npoint = -1;
        return;
    }
    else
    {
        // Handle the case when the current position is valid
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
// Helper function to clear data structures
void MainWindow::ClearDataStructures()
{
    listSource.clear();
    visit.clear();
    listWindow.clear();
}

// Helper function to initialize visit flags
void MainWindow::InitializeVisitFlags()
{
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
}

void MainWindow::WeilerAtherton()
{
    ClearDataStructures();
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

    InitializeVisitFlags();

    Result.clear();
    Result.push_back(vector<Point>()); // Initialize the first contour

    int currentContour = 0;
    int pPolygon = -1;
    int pLine = -1;
    int pPoint = -1;
    bool pMode = SOURCE;

    while (true)
    {
        int nPolygon = -1;
        int nLine = -1;
        int nPoint = -1;
        bool nMode = false;
        int vPolygon = -1;
        int vLine = -1;
        int vPoint = -1;

        FindNextPlace(nPolygon, nLine, nPoint, nMode, pPolygon, pLine, pPoint, pMode, currentContour);

        if (nPolygon == -1 || nLine == -1 || nPoint == -1)
        {
            break;
        }

        if (nMode == WINDOW)
        {
            FindRelatedPlace(WINDOW, listWindow[nPolygon][nLine][nPoint], vPolygon, vLine, vPoint);
        }
        else
        {
            vPolygon = nPolygon;
            vLine = nLine;
            vPoint = nPoint;
        }

        if (visit[vPolygon][vLine][vPoint] == 1)
        {
            Result.push_back(vector<Point>()); // Start a new contour
            currentContour++;
            pPolygon = -1;
            pLine = -1;
            pPoint = -1;
            pMode = SOURCE;
        }
        else
        {
            Point nova = Point(listSource[vPolygon][vLine][vPoint].xCoordinate, listSource[vPolygon][vLine][vPoint].yCoordinate);
            Result[currentContour].push_back(nova);
            visit[vPolygon][vLine][vPoint] = 1;
            pPolygon = nPolygon;
            pLine = nLine;
            pPoint = nPoint;
            pMode = nMode;
        }
    }

    JudgeSpecial();
    repaint();
}

void MainWindow::ResetAll()
{
    mode = SOURCE;
    SourcePoints.clear();
    WindowPoints.clear();
    BufferPoints.clear();
    ui->setupUi(this);
    ui->SourceButton->setStyleSheet("background-color: green; color: white;");
    ui->WindowButton->setStyleSheet("background-color: grey; color: black;");
}

bool MainWindow::HasNoIntersection(bool mode, int num)
{
    const auto& polygonList = (mode == SOURCE) ? listSource[num] : listWindow[num];

    for (const auto& lineIntersections : polygonList)
    {
        if (!lineIntersections.empty())
        {
            return false; // If any line has intersections, return false
        }
    }

    return true; // No intersections found for any line
}


//bool MainWindow::JudgeAdd(bool mode, int num)
//{
//    if (mode == SOURCE)
//    {
//        for (int i = 0; i < WindowPoints.size(); i++)
//        {
//            auto current = SourcePoints[num].begin();
//            while (current != SourcePoints[num].end())
//            {
//                Point c_point = Point(current->x, current->y);
//                if (i != OutWindow)
//                {
//                    if (JudgeInside(c_point, WindowPoints[i])) return 0;
//                }
//                else
//                {
//                    if (!JudgeInside(c_point, WindowPoints[i])) return 0;
//                }
//                current++;
//            }
//        }
//    }
//    else
//    {
//        for (int i = 0; i < SourcePoints.size(); i++)
//        {
//            auto current = WindowPoints[num].begin();
//            while (current != WindowPoints[num].end())
//            {
//                Point c_point = Point(current->x, current->y);
//                if (i != OutSource)
//                {
//                    if (JudgeInside(c_point, SourcePoints[i])) return 0;
//                }
//                else
//                {

//                    if (!JudgeInside(c_point, SourcePoints[i])) return 0;
//                }
//                current++;
//            }
//        }
//    }
//    return 1;
//}

bool MainWindow::JudgeAdd(bool mode, int num)
{
    bool shouldAdd = true; // Initialize the flag as true

    if (mode == SOURCE)
    {
        for (int i = 0; i < WindowPoints.size(); i++)
        {
            const bool isInside = (i != OutWindow);

            if (ShouldNotAdd(SourcePoints[num], WindowPoints[i], isInside))
            {
                shouldAdd = false; // Update the flag if necessary
                break; // No need to check other windows
            }
        }
    }
    else
    {
        for (int i = 0; i < SourcePoints.size(); i++)
        {
            const bool isInside = (i != OutSource);

            if (ShouldNotAdd(WindowPoints[num], SourcePoints[i], isInside))
            {
                shouldAdd = false; // Update the flag if necessary
                break; // No need to check other sources
            }
        }
    }

    return shouldAdd;
}

// Helper function to determine if addition should be avoided
bool MainWindow::ShouldNotAdd(const list<Point>& polygonA, const list<Point>& polygonB, bool isInside)
{
    for (const Point& point : polygonA)
    {
        if ((isInside && !JudgeInside(point, polygonB)) ||
            (!isInside && JudgeInside(point, polygonB)))
        {
            return true; // Avoid addition if the condition is met
        }
    }

    return false; // Allow addition by default
}


//void MainWindow::JudgeSpecial()
//{
//    for (int i = 0; i < SourcePoints.size(); i++)
//    {
//        if (HasNoIntersection(SOURCE, i))
//        {
//            if (JudgeAdd(SOURCE, i))
//            {
//                vector<Point> nova_list;
//                nova_list.clear();
//                Result.push_back(nova_list);
//                auto current = SourcePoints[i].begin();
//                while (current != SourcePoints[i].end())
//                {
//                    Point c_point = Point(current->x, current->y);
//                    Result[Result.size() - 1].push_back(c_point);
//                    current++;
//                }
//            }
//        }
//    }

//    for (int i = 0; i < WindowPoints.size(); i++)
//    {
//        if (HasNoIntersection(WINDOW, i))
//        {
//            if (JudgeAdd(WINDOW, i))
//            {
//                vector<Point> nova_list;
//                nova_list.clear();
//                Result.push_back(nova_list);
//                auto current = WindowPoints[i].begin();
//                while (current != WindowPoints[i].end())
//                {
//                    Point c_point = Point(current->x, current->y);
//                    Result[Result.size() - 1].push_back(c_point);
//                    current++;
//                }
//            }
//        }
//    }
//}
void MainWindow::JudgeSpecial()
{
    // Process source polygons
    for (int i = 0; i < SourcePoints.size(); i++)
    {
        if (HasNoIntersection(SOURCE, i) && JudgeAdd(SOURCE, i))
        {
            AddPolygonToResult(SourcePoints[i]);
        }
    }

    // Process window polygons
    for (int i = 0; i < WindowPoints.size(); i++)
    {
        if (HasNoIntersection(WINDOW, i) && JudgeAdd(WINDOW, i))
        {
            AddPolygonToResult(WindowPoints[i]);
        }
    }
}

// Helper function to add a polygon to the result
void MainWindow::AddPolygonToResult(const list<Point>& polygon)
{
    vector<Point> novaList;

    for (const Point& point : polygon)
    {
        novaList.push_back(point);
    }

    Result.push_back(novaList);
}
