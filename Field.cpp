#include "Field.hpp"
#include "Cell.hpp"

#include <QDebug>

Field::Field():
    QObject()
{
    m_state = StateIdle;
}

void Field::setSize(int width, int height)
{
    m_resetInProgress = true;
    emit progressChanged(resetInProgress());
    m_width = width;
    emit widthChanged(width);
    m_height = height;
    emit heightChanged(height);

    for (Cell *cell : m_cells) {
        delete cell;
    }
    m_cells.clear();

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            Cell *cell = new Cell(x, y);
            connect(cell, SIGNAL(opened(int,int)), this, SLOT(onCellOpened(int,int)));
            connect(cell, SIGNAL(markChanged(Mark)), this, SLOT(onCellMarkChanged()));
            m_cells.append(cell);
        }
    }
    m_resetInProgress = false;
    emit progressChanged(resetInProgress());
}

void Field::setNumberOfMines(int number)
{
    m_numberOfMines = number;
}

void Field::generate(int x, int y)
{
    qsrand(10);

    Cell *banned = cellAt(x, y);
    QVector<Cell*> bannedCells = banned->getNeighbors();
    bannedCells.append(banned);

    int minesToPlace = m_numberOfMines;

    while (minesToPlace > 0) {
        Cell *cell = m_cells.at(qrand() % m_cells.count());

        if (cell->haveMine() || bannedCells.contains(cell)) {
            continue;
        }

        cell->setHaveMine(true);
        --minesToPlace;
    }

    m_generated = true;
    setState(StateStarted);
}

void maybeAddCell(QVector<Cell*> *vector, Cell *cell)
{
    if (cell) {
        vector->append(cell);
    }
}

void Field::startNewGame()
{
    m_generated = false;
    m_numberOfOpenedCells = 0;
    setState(StateIdle);

    for (int i = 0; i < m_cells.size();i++) {
        m_cells[i]->reset();

        QVector<Cell*> neighbors;
        for (int x = m_cells[i]->x() - 1; x <= m_cells[i]->x() + 1; ++x) {
            maybeAddCell(&neighbors, cellAt(x, m_cells[i]->y() - 1));
            maybeAddCell(&neighbors, cellAt(x, m_cells[i]->y() + 1));
        }
        maybeAddCell(&neighbors, cellAt(m_cells[i]->x() - 1, m_cells[i]->y()));
        maybeAddCell(&neighbors, cellAt(m_cells[i]->x() + 1, m_cells[i]->y()));

        m_cells[i]->setNeighbors(neighbors);
    }
}

void Field::lose()
{
    for (int i = 0; i < m_cells.size(); i++) {
        m_cells[i]->open();
    }
    setState(StateEnded);
}

Cell *Field::cellAt(int x, int y) const
{
    if (x < 0 || x >= m_width) {
        return 0;
    }
    if (y < 0 || y >= m_height) {
        return 0;
    }

    return m_cells.at(x + y * m_width);
}

void Field::onCellOpened(int x, int y)
{
    m_numberOfOpenedCells++;

    if (!isGenerated()) {
        generate(x, y);
    }

    if (cellAt(x, y)->haveMine()){
        lose();
    }

    if (m_numberOfOpenedCells == m_cells.count() - m_numberOfMines) {
        win();
    }
}

void Field::onCellMarkChanged()
{
    int mark = 0;
    for (int i = 0; i < m_cells.size(); i++) {
        if(m_cells[i]->mark() == Cell::MarkFlagged) {
            mark++;
        }
    }
    m_numberOfFlags = mark;
    emit numberOfFlagsChanged(numberOfFlags());
}

void Field::win()
{
    qDebug() << "Win!";
    for (int i = 0; i < m_cells.size(); i++) {
        m_cells[i]->reveal();
    }
    setState(StateEnded);
}

void Field::setState(Field::State newState)
{
    if (m_state == newState) {
        return;
    }

    m_state = newState;
    emit stateChanged(state());
}
