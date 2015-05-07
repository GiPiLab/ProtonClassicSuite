#include "treemapwidget.h"

#include <QMouseEvent>
#include <QPainter>

TreeMapWidget::TreeMapWidget(QWidget *parent) : QWidget(parent)
{
    root = new TreeMap();
    highlight=nullptr;
    setMouseTracking(true);
    installEventFilter(this);
    setContentsMargins(0,0,0,0);
}

TreeMapWidget::~TreeMapWidget()
{
    if(root!=nullptr)
        delete root;
}

void TreeMapWidget::setData(PCx_Audit *audit,MODES::DFRFDIRI mode,PCx_Audit::ORED ored,unsigned int year,unsigned int nodeId)
{
    if(audit==nullptr)
    {
        qFatal("Assertion failed: null audit ptr");
    }

    if(year!=0 && !audit->getYears().contains(year))
    {
        qFatal("Assertion failed: invalid year");
    }

    PCx_Tree *tree=audit->getAttachedTree();
    if(tree==nullptr)
    {
        qFatal("Assertion failed: null tree");
    }
    if(nodeId!=0 && tree->isLeaf(nodeId))
        return;

    root->clear();

    if(nodeId==0)
    {
        QList<unsigned int> years=audit->getYears();
        foreach(unsigned int aYear,years)
        {
            TreeMap *group=root->insert(QString::number(aYear),0,aYear,0.0);

            QList<unsigned int> children=tree->getChildren(1);

            foreach(unsigned int child,children)
            {
                group->insert(tree->getNodeName(child),child,aYear,NUMBERSFORMAT::fixedPointToDouble(audit->getNodeValue(child,mode,ored,aYear)));
            }
        }
    }
    else
    {
            TreeMap *group=root->insert(tree->getNodeName(nodeId),nodeId,year,0.0);

            QList<unsigned int> children=tree->getChildren(nodeId);

            foreach(unsigned int child,children)
            {
                group->insert(tree->getNodeName(child),child,year,NUMBERSFORMAT::fixedPointToDouble(audit->getNodeValue(child,mode,ored,year)));
            }
    }

    resizeEvent(nullptr);
    repaint();
}

bool TreeMapWidget::eventFilter(QObject *object, QEvent *e)
{
    Q_UNUSED(object);
    if (e->type() == QEvent::MouseMove)
    {
        QPoint pos = static_cast<QMouseEvent*>(e)->pos();
        TreeMap *underMouse = nullptr;

        foreach (TreeMap *first, root->getChildren())
            if ((underMouse = first->findAt(pos)) != nullptr)
                break;

        if (underMouse && highlight != underMouse)
        {
            highlight = underMouse;
            repaint();
            emit highlighted(underMouse->getName(),underMouse->getId(), underMouse->getYear(), underMouse->getValue());
            return true;
        }
    }
    else if (e->type() == QEvent::Leave)
    {
        highlight = nullptr;
        repaint();
        return true;
    }
    else if (e->type() == QEvent::MouseButtonPress)
    {
        QPoint pos = static_cast<QMouseEvent*>(e)->pos();
        Qt::MouseButton button = static_cast<QMouseEvent*>(e)->button();

        if (button == Qt::LeftButton)
        {
            TreeMap *underMouse = nullptr;
            foreach (TreeMap *first, root->getChildren())
                if ((underMouse = first->findAt(pos)) != nullptr)
                    break;

            if (underMouse)
            {
                emit clicked(underMouse->getName(),underMouse->getId(), underMouse->getYear(), underMouse->getValue());
            }
        }
    }
    return false;
}

void TreeMapWidget::paintEvent(QPaintEvent *paintEvent)
{
    Q_UNUSED(paintEvent);
    if (!root) return;

    QFont font;

    QPainter painter(this);
    QColor color(0,0,0,0);
    QPen pen(color);
    pen.setWidth(10);
    QBrush brush(color);
    painter.setBrush(brush);
    painter.setPen(pen);

    QRect rootRect=root->getRect();
    painter.drawRect(rootRect.x()+4,
                     rootRect.y()+4,
                     rootRect.width()-8,
                     rootRect.height()-8);

    pen.setWidth(5);
    pen.setColor(color);
    painter.setPen(pen);

    int n=1;
    QColor cHSV, cRGB;
    double factor = double(1) / double(root->getChildren().count());
    foreach (TreeMap *first, root->getChildren())
    {
        cHSV.setHsv((double)255 / (factor*n++), 255, 150);
        cRGB = cHSV.convertTo(QColor::Rgb);
        brush.setColor(cRGB);
        painter.setBrush(brush);
        painter.drawRect(first->getRect());
    }

    QPen textPen(Qt::white);
    font.setPointSize(8);
    painter.setFont(font);

    color = Qt::darkGray;
    color.setAlpha(127);
    brush.setColor(color);
    QColor hcolor(Qt::lightGray);
    hcolor.setAlpha(127);
    QBrush hbrush(hcolor);
    QRect textRect;
    QRect demandedTextRect;

    foreach (TreeMap *first, root->getChildren())
    {
        foreach (TreeMap *second, first->getChildren())
        {
            if (second == highlight) painter.setBrush(hbrush);
            else painter.setBrush(brush);

            painter.setPen(Qt::NoPen);
            QRect secondRect=second->getRect();
            painter.drawRect(secondRect.x()+2,
                             secondRect.y()+2,
                             secondRect.width()-4,
                             secondRect.height()-4);

            textRect.setRect(secondRect.x()+2, secondRect.y()+2,secondRect.width()-4, secondRect.height()-4 );
            font.setPointSize(10);
            painter.setFont(font);
            painter.setPen(textPen);
            demandedTextRect = painter.boundingRect(textRect, Qt::AlignTop|Qt::AlignLeft, second->getName());
            if (!textRect.contains(demandedTextRect))
            {
                font.setPointSize(8);
                painter.setFont(font);
                demandedTextRect = painter.boundingRect(textRect, Qt::AlignTop|Qt::AlignLeft, second->getName());
                if (!textRect.contains(demandedTextRect))
                {
                    font.setPointSize(7);
                    painter.setFont(font);                    
                }
            }
            painter.drawText(textRect, Qt::AlignTop|Qt::AlignLeft|Qt::TextWordWrap, second->getName());
        }
    }

    textPen.setColor(Qt::black);
    font.setPointSize(16);
    painter.setFont(font);
    painter.setPen(textPen);
    foreach (TreeMap *first, root->getChildren())
    {
        painter.drawText(first->getRect(),
                         Qt::AlignVCenter|Qt::AlignHCenter,
                         first->getName());
    }
}

void TreeMapWidget::resizeEvent(QResizeEvent *resizeEvent)
{
    Q_UNUSED(resizeEvent);
    if (root) root->layout(QRect(9,9,geometry().width()-18, geometry().height()-18));
}
