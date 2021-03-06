/*!
 * \file painter_attribute_data_filler.hpp
 * \brief file painter_attribute_data_filler.hpp
 *
 * Copyright 2016 by Intel.
 *
 * Contact: kevin.rogovin@gmail.com
 *
 * This Source Code Form is subject to the
 * terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with
 * this file, You can obtain one at
 * http://mozilla.org/MPL/2.0/.
 *
 * \author Kevin Rogovin <kevin.rogovin@gmail.com>
 *
 */


#pragma once

#include <fastuidraw/util/util.hpp>
#include <fastuidraw/util/c_array.hpp>
#include <fastuidraw/painter/attribute_data/painter_attribute.hpp>

namespace fastuidraw
{
/*!\addtogroup PainterAttribute
 * @{
 */

  /*!
   * \brief
   * A PainterAttributeDataFiller is the interfaceto fill the
   * data held by a \ref PainterAttributeData
   */
  class PainterAttributeDataFiller:noncopyable
  {
  public:
    virtual
    ~PainterAttributeDataFiller()
    {}

    /*!
     * To be implemented by a derived class to specify
     * how many attributes, indices and chunks the
     * PainterAttributeDataFiller will fill. All attributes
     * and indices are on a common array. A chunk is a
     * selection of attribute and index data. Each
     * of the outputs is initialized as zero.
     * \param[out] number_attributes number of total attributes to be set
     * \param[out] number_indices number of total indices to be set
     * \param[out] number_attribute_chunks number of attribute chunks to be set
     * \param[out] number_index_chunks number of index chunks to be set
     * \param[out] number_z_ranges the number of z-ranges to be set;
     *                             z-ranges of PainterAttribtueData are
     *                             accessed by PainterAttributeData::z_ranges()
     *                             and PainterAttributeData::z_range().
     */
    virtual
    void
    compute_sizes(unsigned int &number_attributes,
                  unsigned int &number_indices,
                  unsigned int &number_attribute_chunks,
                  unsigned int &number_index_chunks,
                  unsigned int &number_z_ranges) const = 0;

    /*!
     * To be implemented by a derived class to fill data.
     * \param attributes location to which to place attributes
     * \param indices location to which to place indices
     * \param attrib_chunks location to which to fill attribute chunks;
     *                      each element of attrib_chunks must be a
     *                      sub-array of attributes. Initialized so that
     *                      each element is an empty array.
     * \param index_chunks location to which to fill index chunks;
     *                     each element of attrib_chunks must be a
     *                     sub-array of indices. Initialized so that
     *                     each element is an empty array.
     * \param zranges location to which to fill the z-range values
     *                (PainterAttributeData::z_ranges()).
     * \param index_adjusts location to which to fill the index adjust value
     *                      (PainterAttributeData::index_adjust_chunks()).
     */
    virtual
    void
    fill_data(c_array<PainterAttribute> attributes,
              c_array<PainterIndex> indices,
              c_array<c_array<const PainterAttribute> > attrib_chunks,
              c_array<c_array<const PainterIndex> > index_chunks,
              c_array<range_type<int> > zranges,
              c_array<int> index_adjusts) const = 0;
  };
/*! @} */
}
