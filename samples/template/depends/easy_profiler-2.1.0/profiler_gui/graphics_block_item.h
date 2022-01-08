/************************************************************************
* file name         : graphics_block_item.h
* ----------------- :
* creation time     : 2016/09/15
* author            : Victor Zarubkin
* email             : v.s.zarubkin@gmail.com
* ----------------- :
* description       : The file contains declaration of GraphicsBlockItem - an item
*                   : used to draw profiler blocks on graphics scene.
* ----------------- :
* license           : Lightweight profiler library for c++
*                   : Copyright(C) 2016-2019  Sergey Yagovtsev, Victor Zarubkin
*                   :
*                   : Licensed under either of
*                   :     * MIT license (LICENSE.MIT or http://opensource.org/licenses/MIT)
*                   :     * Apache License, Version 2.0, (LICENSE.APACHE or http://www.apache.org/licenses/LICENSE-2.0)
*                   : at your option.
*                   :
*                   : The MIT License
*                   :
*                   : Permission is hereby granted, free of charge, to any person obtaining a copy
*                   : of this software and associated documentation files (the "Software"), to deal
*                   : in the Software without restriction, including without limitation the rights 
*                   : to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies 
*                   : of the Software, and to permit persons to whom the Software is furnished 
*                   : to do so, subject to the following conditions:
*                   : 
*                   : The above copyright notice and this permission notice shall be included in all 
*                   : copies or substantial portions of the Software.
*                   : 
*                   : THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
*                   : INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
*                   : PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE 
*                   : LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
*                   : TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE 
*                   : USE OR OTHER DEALINGS IN THE SOFTWARE.
*                   : 
*                   : The Apache License, Version 2.0 (the "License")
*                   :
*                   : You may not use this file except in compliance with the License.
*                   : You may obtain a copy of the License at
*                   :
*                   : http://www.apache.org/licenses/LICENSE-2.0
*                   :
*                   : Unless required by applicable law or agreed to in writing, software
*                   : distributed under the License is distributed on an "AS IS" BASIS,
*                   : WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*                   : See the License for the specific language governing permissions and
*                   : limitations under the License.
************************************************************************/

#ifndef GRAPHICS_BLOCK_ITEM_H
#define GRAPHICS_BLOCK_ITEM_H

#include <stdlib.h>

#include <QGraphicsItem>
#include <QRectF>
#include <QString>

#include <easy/reader.h>

#include "common_types.h"

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

class BlocksGraphicsView;

class GraphicsBlockItem : public QGraphicsItem
{
    using Children    = profiler_gui::EasyItems;
    using DrawIndexes = std::vector<uint32_t>;
    using RightBounds = std::vector<qreal>;
    using Sublevels   = std::vector<Children>;

    const profiler::BlocksTreeRoot&  m_thread; ///< Reference to the root profiler block (thread block). Used by ProfTreeWidget to restore hierarchy.

    DrawIndexes               m_levelsIndexes; ///< Indexes of first item on each level from which we must start painting
    RightBounds                 m_rightBounds; ///<
    Sublevels                        m_levels; ///< Arrays of items for each level

    QRectF                     m_boundingRect; ///< boundingRect (see QGraphicsItem)
    QString                      m_threadName; ///<
    uint8_t                           m_index; ///< This item's index in the list of items of BlocksGraphicsView

public:

    explicit GraphicsBlockItem(uint8_t _index, const profiler::BlocksTreeRoot& _root);
    ~GraphicsBlockItem() override;

    // Public virtual methods

    QRectF boundingRect() const override;

    void paint(QPainter* _painter, const QStyleOptionGraphicsItem* _option, QWidget* _widget = nullptr) override;

public:

    // Public non-virtual methods

    void validateName();

    const profiler::BlocksTreeRoot& root() const;
    const QString& threadName() const;

    QRect getRect() const;

    void setBoundingRect(qreal x, qreal y, qreal w, qreal h);
    void setBoundingRect(const QRectF& _rect);

    ::profiler::thread_id_t threadId() const;

    ///< Returns number of levels
    uint8_t levels() const;

    float levelY(uint8_t _level) const;

    /** \brief Sets number of levels.
    
    \note Must be set before doing anything else.
    
    \param _levels Desired number of levels */
    void setLevels(uint8_t _levels);

    /** \brief Reserves memory for desired number of items on specified level.
    
    \param _level Index of the level
    \param _items Desired number of items on this level */
    void reserve(uint8_t _level, unsigned int _items);

    /**\brief Returns reference to the array of items of specified level.
    
    \param _level Index of the level */
    const Children& items(uint8_t _level) const;

    /**\brief Returns reference to the item with required index on specified level.
    
    \param _level Index of the level
    \param _index Index of required item */
    const ::profiler_gui::EasyBlockItem& getItem(uint8_t _level, unsigned int _index) const;

    /**\brief Returns reference to the item with required index on specified level.

    \param _level Index of the level
    \param _index Index of required item */
    ::profiler_gui::EasyBlockItem& getItem(uint8_t _level, unsigned int _index);

    /** \brief Adds new item to required level.
    
    \param _level Index of the level
    
    \retval Index of the new created item */
    unsigned int addItem(uint8_t _level);

    /** \brief Finds top-level blocks which are intersects with required selection zone.

    \note Found blocks will be added into the array of selected blocks.
    
    \param _left Left bound of the selection zone
    \param _right Right bound of the selection zone
    \param _blocks Reference to the array of selected blocks */
    void getBlocks(qreal _left, qreal _right, ::profiler_gui::TreeBlocks& _blocks) const;

    const ::profiler_gui::EasyBlock* intersect(const QPointF& _pos, ::profiler::block_index_t& _blockIndex) const;
    const ::profiler_gui::EasyBlock* intersectEvent(const QPointF& _pos) const;

private:

    ///< Returns pointer to the BlocksGraphicsView widget.
    const BlocksGraphicsView* view() const;

#ifdef EASY_GRAPHICS_ITEM_RECURSIVE_PAINT
    void paintChildren(const float _minWidth, const int _narrowSizeHalf, const uint8_t _levelsNumber, QPainter* _painter, struct EasyPainterInformation& p, ::profiler_gui::EasyBlockItem& _item, const ::profiler_gui::EasyBlock& _itemBlock, RightBounds& _rightBounds, uint8_t _level, int8_t _mode);
#endif

public:

    // Public inline methods

    ///< Returns this item's index in the list of graphics items of BlocksGraphicsView
    uint8_t index() const {
        return m_index;
    }

}; // END of class GraphicsBlockItem.

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#endif // GRAPHICS_BLOCK_ITEM_H
