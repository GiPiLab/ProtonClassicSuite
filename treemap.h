#ifndef TREEMAP_H
#define TREEMAP_H

#include <QString>
#include <QList>
#include <QRect>

class TreeMap
{
public:
    TreeMap(TreeMap *parent = nullptr, const QString &name = "", unsigned int id=0, int year=0, double value = 0.0);
    virtual ~TreeMap();

    TreeMap *insert(const QString &name, unsigned int id=0, int year=0, double value=0.0);
    TreeMap *findAt(const QPoint &pos) const;

    void clear(void);
    void sort(void);

    const TreeMap *getParent() const{return parent;}
    const QList<TreeMap *> getChildren() const{return children;}
    const QRect &getRect() const{return rect;}
    const QString &getName() const{return name;}
    int getId() const{return id;}
    int getYear() const{return year;}
    double getValue() const{return value;}

    void layout(const QRect &rect);

private:

    void layout(QList<TreeMap*> items, int start, int end, const QRect &bounds);
    void squarifyLayout(QList<TreeMap *>items,const QRect &bounds);
    double aspect(double _big, double _small, double a, double b) const;
    double normAspect(double _big, double _small, double a, double b) const;
    void slicelayout(QList<TreeMap*> items, int start, int end, const QRect &bounds);

    static bool lessThan(const TreeMap *a,const TreeMap *b);

    TreeMap *parent;
    QString name;
    int id;
    int year;
    double value;
    QList<TreeMap *>children;
    QRect rect;

};

#endif // TREEMAP_H
