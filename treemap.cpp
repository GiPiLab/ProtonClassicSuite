#include "treemap.h"


TreeMap::TreeMap(TreeMap *parent, const QString &name, unsigned int id, int year, double value):parent(parent),name(name),id(id),year(year),value(value)
{
}

TreeMap::~TreeMap()
{
    clear();
}

TreeMap *TreeMap::insert(const QString &name, unsigned int id, int year, double value)
{
    this->value += value;
    for (TreeMap *p = parent; p != nullptr; p = p->parent) p->value += value;

    foreach (TreeMap *x, children)
    {
        if (x->name == name)
        {
            x->value += value;
            return x;
        }
    }
    TreeMap *newone = new TreeMap(this, name, id,year,value);
    children.append(newone);
    return newone;
}

TreeMap *TreeMap::findAt(const QPoint &pos) const
{
    foreach (TreeMap *child, children)
    {
        if (child->rect.contains(pos)) return child;
    }
    return nullptr;
}

void TreeMap::clear(void)
{
    foreach (TreeMap *x, children)
    {
        x->clear();
        delete x;
    }
    children.clear();
    name = "";
    value = 0.0;
}

void TreeMap::sort()
{
    qSort(children.begin(), children.end(), lessThan);
    foreach (TreeMap *child, children) child->sort();
}

void TreeMap::layout(const QRect &rect)
{
    this->rect = rect;
    sort();
    squarifyLayout(children, rect);
}

void TreeMap::layout(QList<TreeMap *> items, int start, int end, const QRect &bounds)
{
    if (start > end) return;

    if (end-start < 2)
    {
        slicelayout(items, start, end, bounds);
        return;
    }

    double x=bounds.x(),
            y=bounds.y(),
            w=bounds.width(),
            h=bounds.height();

    double total=0;
    for(int i=start; i<=end; i++) total += items[i]->value;
    int mid=start;
    double a=items[start]->value/total;
    double b=a;

    if (w<h)
    {
        while (mid<=end)
        {
            double aspect=normAspect(h,w,a,b);
            double q=items[mid]->value/total;

            if (normAspect(h,w,a,b+q)>aspect) break;

            mid++;
            b+=q;
        }

        slicelayout(items,start,mid, QRect(x,y,w,h*b));
        layout(items,mid+1,end, QRect(x,y+h*b,w,h*(1-b)));

    }
    else
    {
        while (mid<=end)
        {
            double aspect=normAspect(w,h,a,b);
            double q=items[mid]->value/total;

            if (normAspect(w,h,a,b+q)>aspect) break;

            mid++;
            b+=q;
        }

        slicelayout(items,start,mid, QRect(x,y,w*b,h));
        layout(items,mid+1,end, QRect(x+w*b,y,w*(1-b),h));
    }
}

void TreeMap::squarifyLayout(QList<TreeMap *> items, const QRect &bounds)
{
    rect = bounds;
    layout(items, 0, items.count()-1, bounds);
    foreach (TreeMap *item, items)
        item->squarifyLayout(item->children, item->rect);
}

double TreeMap::aspect(double _big, double _small, double a, double b) const
{
    return (_big*b)/(_small*a/b);
}

double TreeMap::normAspect(double _big, double _small, double a, double b) const
{
    double x=aspect(_big,_small,a,b);
    if (x<1) return 1/x;
    return x;
}

void TreeMap::slicelayout(QList<TreeMap *> items, int start, int end, const QRect &bounds)
{
    double total=0, accumulator=0;
    for(int i= start; i<= end && i<items.count(); i++) total += items[i]->value;
    Qt::Orientation orientation = (bounds.width() > bounds.height()) ? Qt::Horizontal : Qt::Vertical;

    for (int i=start; i<=end && i<items.count(); i++)
    {
        double factor=items[i]->value/total;
        if (orientation == Qt::Vertical)
        {
            items[i]->rect.setX(bounds.x());
            items[i]->rect.setWidth(bounds.width());
            items[i]->rect.setY(bounds.y()+bounds.height()*(1-accumulator-factor));
            items[i]->rect.setHeight(bounds.height()*factor);
        }
        else
        {
            items[i]->rect.setX(bounds.x()+bounds.width()*(1-accumulator-factor));
            items[i]->rect.setWidth(bounds.width()*factor);
            items[i]->rect.setY(bounds.y());
            items[i]->rect.setHeight(bounds.height());
        }
        accumulator += factor;
    }
}

bool TreeMap::lessThan(const TreeMap *a, const TreeMap *b)
{
    return (a->value) > (b->value);
}
