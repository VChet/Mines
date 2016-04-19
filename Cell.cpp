#include "Cell.hpp"

Cell::Cell(int x, int y):
    QObject()
{
    m_x = x;
    m_y = y;

    m_haveMine = false;
    m_open = false;
    m_mark = MarkNothing;
}

int Cell::minesAround() const
{
    int mines = 0;
    for (Cell *cell : getNeighbors()) {
        if (cell->haveMine()) {
            ++mines;
        }
    }

    return mines;
}

void Cell::setHaveMine(bool haveMine)
{
    m_haveMine = haveMine;
}

void Cell::open()
{
    if (m_open) {
        return;
    }

    m_open = true;

    emit opened(x(), y());

    if (minesAround() == 0) {
        for (Cell *cell : getNeighbors()) {
            cell->open();
        }
    }
}

void Cell::reset()
{
    m_haveMine = false;
    m_open = false;
    m_mark = MarkNothing;
    emit markChanged(m_mark);
}

void Cell::toggleMark()
{
    switch (m_mark) {
    case MarkNothing:
        m_mark = MarkFlagged;
        emit markChanged(m_mark);
        break;
    case MarkFlagged:
        m_mark = MarkQuestioned;
        emit markChanged(m_mark);
        break;
    case MarkQuestioned:
        m_mark = MarkNothing;
        emit markChanged(m_mark);
        break;
    }
}

void Cell::tryToOpenAround()
{
    int mark;

    for (Cell *cell : getNeighbors()) {
        if (cell->mark() == 1) {
            ++mark;
        }
    }

    if (mark == minesAround()){
        for (Cell *cell : getNeighbors()) {
            if (cell->mark() == 0){
                cell->open();
            }
        }
    }
}

void Cell::setNeighbors(const QVector<Cell *> &neighbors)
{
    m_neighbors = neighbors;
}

QVector<Cell *> Cell::getNeighbors() const
{
    return m_neighbors;
}
