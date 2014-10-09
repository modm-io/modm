
#ifndef MAX7219MATRIX_HPP_
#define MAX7219MATRIX_HPP_

#include <xpcc/ui/display/buffered_graphic_display.hpp>

namespace xpcc
{

/**
 * Some light hardware abstractions of MAX7219
 *
 * @tparam	SPI		SPI Interface
 * @tapram	CS		Chip Select of MAX7219
 * @tparam	DRIVERS	Number of chips in daisy chain
 *
 * An arbitrary number of these chips can be daisy-chained
 */
template < typename SPI, typename CS, uint8_t MODULES = 1 >
class Max7219
{
public:

    static void
    initialize()
    {
        // show all 8 digits
        sendByte(MAX7219_REG_SCANLIMIT, 7);

        // using a LED matrix (not digits)
        sendByte(MAX7219_REG_DECODEMODE, 0);

        // no display test
        sendByte(MAX7219_REG_DISPLAYTEST, 0);

        clear();

        // character intensity: range: 0..15
        setBrightness(15);

        // not in shutdown mode (i.e start it up)
        sendByte(MAX7219_REG_SHUTDOWN, 1);
    }

    static void
    clear()
    {
    	// Iterate column 0 to 7 which is addressed 1 to 8
        for (uint8_t col = 1; col <= 8; ++col) {
            sendByte(col, 0);
        };
    }

    /**
     * Set a row of the MAX7219 chips with data
     *
     * @param	data	A pointer to the data, at least MODULES bytes long
     */
    static void
    setRow(uint8_t col, uint8_t * data)
    {
        CS::reset();
        for (uint8_t ii = 0; ii < MODULES; ++ii)
        {
			SPI::writeReadBlocking(col + 1);
			SPI::writeReadBlocking(*data++);
        }
        CS::set();
    }

    /**
     * Set the brightness of the whole matrix.
     *
     * \param intensity range 0 to 15.
     */
    static void
    setBrightness(uint8_t intensity)
    {
        sendByte(MAX7219_REG_INTENSITY, intensity % 16);
    }

private:
    static constexpr uint8_t MAX7219_REG_NOOP        = 0x0;
    static constexpr uint8_t MAX7219_REG_DIGIT0      = 0x1;
    static constexpr uint8_t MAX7219_REG_DIGIT1      = 0x2;
    static constexpr uint8_t MAX7219_REG_DIGIT2      = 0x3;
    static constexpr uint8_t MAX7219_REG_DIGIT3      = 0x4;
    static constexpr uint8_t MAX7219_REG_DIGIT4      = 0x5;
    static constexpr uint8_t MAX7219_REG_DIGIT5      = 0x6;
    static constexpr uint8_t MAX7219_REG_DIGIT6      = 0x7;
    static constexpr uint8_t MAX7219_REG_DIGIT7      = 0x8;
    static constexpr uint8_t MAX7219_REG_DECODEMODE  = 0x9;
    static constexpr uint8_t MAX7219_REG_INTENSITY   = 0xA;
    static constexpr uint8_t MAX7219_REG_SCANLIMIT   = 0xB;
    static constexpr uint8_t MAX7219_REG_SHUTDOWN    = 0xC;
    static constexpr uint8_t MAX7219_REG_DISPLAYTEST = 0xF;

    static void
    sendByte(uint8_t cmd, uint8_t data)
    {
        CS::reset();

        // Write the command multiple times, for each MODULES
        for (uint8_t ii = 0; ii < MODULES; ++ii)
        {
			SPI::writeReadBlocking(cmd);
			SPI::writeReadBlocking(data);
        }
        CS::set();
    }
};


/**
 * @brief	Driver for arrays of LED 8x8 Matrices driven by MAX7219
 *
 * Orientation of the module is with the connector to bottom.
 * Multiple displays are connected from right to left.
 *
 * Layout of the modules for a 24 pixel x 8 pixel display.
 *
 *           CCCC BBBB
 *     +----+----+----+
 *     |    |    |    |
 *     |    |    |    |
 *     +----+----+----+
 *     |MMMM|MMMM|MMMM|
 *     +----+----+----+
 *      CCCC BBBB AAAA
 *
 * MMMM is the Chip
 * AAAA is connected to the microcontroller
 * BBBB is connected to BBBB
 * CCCC is connected to CCCC
 *
 * @tparam	SPI		SPI Interface
 * @tapram	CS		Chip Select of MAX7219
 * @tparam	COLUMNS	Number of modules placed horizontally, from right to left.
 * @tparam	ROW		Number of modules placed vertically
 *
 * 8x8 LED modules with MAX7219 are easily and cheaply available.
 *
 */
template < typename SPI, typename CS, uint8_t COLUMNS = 1, uint8_t ROWS = 1 >
class Max7219matrix : public BufferedGraphicDisplay< 8 * COLUMNS, 8 * ROWS >
{
public:
	/**
	 * \brief	Initialize the display
	 */
	void
	initialize();

	/**
	 * \brief	Update the display with the content of the RAM buffer
	 */
	virtual void
	update();

protected:
	// Instance of a daisy chain of COLUMNS * ROWS modules.
	Max7219< SPI, CS, COLUMNS * ROWS > max;

};
} // xpcc namespace

/* ------------------- Implementation --------------------- */

template < typename SPI, typename CS, uint8_t COLUMNS, uint8_t ROWS >
void
xpcc::Max7219matrix< SPI, CS, COLUMNS, ROWS >::initialize()
{
	max.initialize();
	max.clear();
}

template < typename SPI, typename CS, uint8_t COLUMNS, uint8_t ROWS >
void
xpcc::Max7219matrix< SPI, CS, COLUMNS, ROWS >::update()
{
	// Iterate column 0 to 7 of MAX LED driver
	for (uint8_t ledCol = 0; ledCol < 8; ++ledCol)
	{
		uint8_t buf[COLUMNS * ROWS];
		uint8_t idx = 0;

		// Iterate over display buffer
		// Luckily each LED Matrix has as many pixels vertical
		// as pixels are stored in one byte.

		// col is the column of LED modules and
		// a group of eight pixels horizontal
		for (uint8_t col = 0; col < COLUMNS; ++col)
		{
			// row is the row of LED modules and
			// a group of eight pixels vertical
			for (uint8_t row = 0; row < ROWS; ++row)
			{
				buf[idx] = this->buffer[col * 8 + ledCol][row];
				++idx;
			}
		}

		max.setRow(ledCol, buf);
	}
}

#endif /* MAX7219MATRIX_HPP_ */
