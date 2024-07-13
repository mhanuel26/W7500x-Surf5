#ifndef __SURF5FLASH_H_
#define __SURF5FLASH_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/*!
 * \brief Erases a memory area
 * \param addr Memory area to erase
 * \param nofBytes Number of bytes to erase
 * \return Error code, ERR_OK if everything is fine
 */
uint8_t Surf5Flash_Erase(void *addr, size_t nofBytes);

/*!
 * \brief Program the flash memory with data
 * \param addr Address where to store the data
 * \param data Pointer to the data
 * \param dataSize Number of data bytes
 * \return Error code, ERR_OK if everything is fine
 */
uint8_t Surf5Flash_Program(void *addr, const void *data, size_t dataSize);


/*!
 * \brief Read the flash memory
 * \param addr Address where to store the data
 * \param data Pointer where to store the data
 * \param dataSize Number of data bytes
 * \return Error code, ERR_OK if everything is fine
 */
uint8_t Surf5Flash_Read(const void *addr, void *data, size_t dataSize);


#endif

