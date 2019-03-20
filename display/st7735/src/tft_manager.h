#ifndef TFT_MANAGER_H
#define TFT_MANAGER_H

#include <string>
#include <list>

#include "tft_field.h"

/**
 * TFT_manager class.
 *
 * The class remembers all TFT_fields in a list and handles the refresh on the display
 */
class TFT_manager
{
  private:
    std::list<TFT_field*> _field_list;

  public:
    /** @brief Constructor of the class
     *
     *  @return should not return a value
     *
     */
    TFT_manager();

    /** @brief Add a field to the manager list
     *
     *  @param *field pointer to TFT_field
     *
     *  @return void
     *
     */
    void add(TFT_field* field);

    /** @brief Refresh all fields in the manager list
     *
     *  @return void
     *
     */
    void refresh (void);
};

#endif // TFT_MANAGER_H
