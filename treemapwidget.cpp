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


void TreeMapWidget::setDataGroupByYears(PCx_Audit* audit,MODES::DFRFDIRI mode,PCx_Audit::ORED ored, unsigned int nodeId)
{
    if(audit==nullptr)
    {
        qFatal("Assertion failed: null audit ptr");
    }

    PCx_Tree *tree=audit->getAttachedTree();
    if(tree==nullptr)
    {
        qFatal("Assertion failed: null tree");
    }

    root->clear();

    QList<unsigned int> years=audit->getYears();
    foreach(unsigned int aYear,years)
    {
        TreeMap *group=root->insert(QString::number(aYear),0,aYear,0.0);

        if(tree->isLeaf(nodeId))
        {
            qint64 val=audit->getNodeValue(nodeId,mode,ored,aYear);
            if(val>0)
                group->insert(tree->getNodeName(nodeId),nodeId,aYear,NUMBERSFORMAT::fixedPointToDouble(val));
        }
        else
        {
            QList<unsigned int> children=tree->getChildren(nodeId);

            foreach(unsigned int child,children)
            {
                qint64 val=audit->getNodeValue(child,mode,ored,aYear);
                if(val>0)
                    group->insert(tree->getNodeName(child),child,aYear,NUMBERSFORMAT::fixedPointToDouble(val));
            }
        }
    }

    resizeEvent(nullptr);
    repaint();
}

void TreeMapWidget::setDataGroupByModes(PCx_Audit *audit,PCx_Audit::ORED ored, unsigned int year, unsigned int nodeId)
{
    if(audit==nullptr)
    {
        qFatal("Assertion failed: null audit ptr");
    }

    if(!audit->getYears().contains(year))
    {
        qFatal("Assertion failed: invalid year");
    }

    PCx_Tree *tree=audit->getAttachedTree();
    if(tree==nullptr)
    {
        qFatal("Assertion failed: null tree");
    }

    root->clear();

    TreeMap *groupDF=root->insert("DF");
    TreeMap *groupRF=root->insert("RF");
    TreeMap *groupDI=root->insert("DI");
    TreeMap *groupRI=root->insert("RI");
    if(tree->isLeaf(nodeId))
    {
        qint64 df,rf,di,ri;
        df=audit->getNodeValue(nodeId,MODES::DFRFDIRI::DF,ored,year);
        rf=audit->getNodeValue(nodeId,MODES::DFRFDIRI::RF,ored,year);
        di=audit->getNodeValue(nodeId,MODES::DFRFDIRI::DI,ored,year);
        ri=audit->getNodeValue(nodeId,MODES::DFRFDIRI::RI,ored,year);
        if(df>0)
            groupDF->insert(tree->getNodeName(nodeId),nodeId,year,NUMBERSFORMAT::fixedPointToDouble(df));
        if(rf>0)
            groupRF->insert(tree->getNodeName(nodeId),nodeId,year,NUMBERSFORMAT::fixedPointToDouble(rf));
        if(di>0)
            groupDI->insert(tree->getNodeName(nodeId),nodeId,year,NUMBERSFORMAT::fixedPointToDouble(di));
        if(ri>0)
            groupRI->insert(tree->getNodeName(nodeId),nodeId,year,NUMBERSFORMAT::fixedPointToDouble(ri));
    }
    else
    {

        QList<unsigned int> children=tree->getChildren(nodeId);
        foreach(unsigned int child,children)
        {
            qint64 df,rf,di,ri;
            df=audit->getNodeValue(child,MODES::DFRFDIRI::DF,ored,year);
            rf=audit->getNodeValue(child,MODES::DFRFDIRI::RF,ored,year);
            di=audit->getNodeValue(child,MODES::DFRFDIRI::DI,ored,year);
            ri=audit->getNodeValue(child,MODES::DFRFDIRI::RI,ored,year);
            if(df>0)
                groupDF->insert(tree->getNodeName(child),child,year,NUMBERSFORMAT::fixedPointToDouble(df));
            if(rf>0)
                groupRF->insert(tree->getNodeName(child),child,year,NUMBERSFORMAT::fixedPointToDouble(rf));
            if(di>0)
                groupDI->insert(tree->getNodeName(child),child,year,NUMBERSFORMAT::fixedPointToDouble(di));
            if(ri>0)
                groupRI->insert(tree->getNodeName(child),child,year,NUMBERSFORMAT::fixedPointToDouble(ri));
        }
    }

    resizeEvent(nullptr);
    repaint();
}

void TreeMapWidget::setDataGroupByORED(PCx_Audit *audit, MODES::DFRFDIRI mode, unsigned int year, unsigned int nodeId)
{
    if(audit==nullptr)
    {
        qFatal("Assertion failed: null audit ptr");
    }

    if(!audit->getYears().contains(year))
    {
        qFatal("Assertion failed: invalid year");
    }

    PCx_Tree *tree=audit->getAttachedTree();
    if(tree==nullptr)
    {
        qFatal("Assertion failed: null tree");
    }

    root->clear();

    TreeMap *groupO=root->insert(PCx_Audit::OREDtoCompleteString(PCx_Audit::ORED::OUVERTS));
    TreeMap *groupR=root->insert(PCx_Audit::OREDtoCompleteString(PCx_Audit::ORED::REALISES));
    TreeMap *groupE=root->insert(PCx_Audit::OREDtoCompleteString(PCx_Audit::ORED::ENGAGES));
    TreeMap *groupD=root->insert(PCx_Audit::OREDtoCompleteString(PCx_Audit::ORED::DISPONIBLES));

    if(tree->isLeaf(nodeId))
    {
        qint64 o,r,e,d;
        o=audit->getNodeValue(nodeId,mode,PCx_Audit::ORED::OUVERTS,year);
        r=audit->getNodeValue(nodeId,mode,PCx_Audit::ORED::REALISES,year);
        e=audit->getNodeValue(nodeId,mode,PCx_Audit::ORED::ENGAGES,year);
        d=audit->getNodeValue(nodeId,mode,PCx_Audit::ORED::DISPONIBLES,year);


        if(o>0)
            groupO->insert(tree->getNodeName(nodeId),nodeId,year,NUMBERSFORMAT::fixedPointToDouble(o));
        if(r>0)
            groupR->insert(tree->getNodeName(nodeId),nodeId,year,NUMBERSFORMAT::fixedPointToDouble(r));
        if(e>0)
            groupE->insert(tree->getNodeName(nodeId),nodeId,year,NUMBERSFORMAT::fixedPointToDouble(e));
        if(d>0)
            groupD->insert(tree->getNodeName(nodeId),nodeId,year,NUMBERSFORMAT::fixedPointToDouble(d));

    }

    else
    {
        QList<unsigned int> children=tree->getChildren(nodeId);
        foreach(unsigned int child,children)
        {
            qint64 o,r,e,d;
            o=audit->getNodeValue(child,mode,PCx_Audit::ORED::OUVERTS,year);
            r=audit->getNodeValue(child,mode,PCx_Audit::ORED::REALISES,year);
            e=audit->getNodeValue(child,mode,PCx_Audit::ORED::ENGAGES,year);
            d=audit->getNodeValue(child,mode,PCx_Audit::ORED::DISPONIBLES,year);


            if(o>0)
                groupO->insert(tree->getNodeName(child),child,year,NUMBERSFORMAT::fixedPointToDouble(o));
            if(r>0)
                groupR->insert(tree->getNodeName(child),child,year,NUMBERSFORMAT::fixedPointToDouble(r));
            if(e>0)
                groupE->insert(tree->getNodeName(child),child,year,NUMBERSFORMAT::fixedPointToDouble(e));
            if(d>0)
                groupD->insert(tree->getNodeName(child),child,year,NUMBERSFORMAT::fixedPointToDouble(d));
        }
    }


    resizeEvent(nullptr);
    repaint();
}

void TreeMapWidget::setDataGroupByNode(PCx_Audit *audit,MODES::DFRFDIRI mode,PCx_Audit::ORED ored,unsigned int year,unsigned int nodeId)
{
    if(audit==nullptr)
    {
        qFatal("Assertion failed: null audit ptr");
    }

    if(!audit->getYears().contains(year))
    {
        qFatal("Assertion failed: invalid year");
    }

    PCx_Tree *tree=audit->getAttachedTree();
    if(tree==nullptr)
    {
        qFatal("Assertion failed: null tree");
    }

    root->clear();

    if(tree->isLeaf(nodeId))
    {
        TreeMap *group=root->insert("",nodeId,year,0.0);
        qint64 val=audit->getNodeValue(nodeId,mode,ored,year);
        if(val>0)
            group->insert(tree->getNodeName(nodeId),nodeId,year,NUMBERSFORMAT::fixedPointToDouble(val));
    }

    else
    {
        TreeMap *group=root->insert(tree->getNodeName(nodeId),nodeId,year,0.0);
        QList<unsigned int> children=tree->getChildren(nodeId);

        foreach(unsigned int child,children)
        {
            qint64 val=audit->getNodeValue(child,mode,ored,year);
            if(val>0)
                group->insert(tree->getNodeName(child),child,year,NUMBERSFORMAT::fixedPointToDouble(val));
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
    QColor color(127,127,127,50);
    QPen pen(color);
    QBrush brush(color);
    painter.setBrush(brush);
    painter.setPen(Qt::NoPen);

    int n=1;
    QColor cHSV, cRGB;
    double factor = 1.0/ (double)(root->getChildren().count());
    foreach (TreeMap *first, root->getChildren())
    {
        cHSV.setHsv((double)255 / (factor*n++), 255, 190);
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
            painter.drawRect(secondRect.x(),
                             secondRect.y(),
                             secondRect.width(),
                             secondRect.height());

            textRect.setRect(secondRect.x()+5, secondRect.y()+5,secondRect.width()-10, secondRect.height()-10);
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
    if (root) root->layout(QRect(0,0,geometry().width(), geometry().height()));
}
