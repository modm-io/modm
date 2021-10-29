// OPTIMIZE BufferMemory<Monochrome, R> for speed

template<Size R>
template<template<typename> class Accessor>
void
BufferMemory<Monochrome, R>::writeImage(ImageAccessor<C, Accessor> accessor, Point placement)
{
	const Section clipping = this->getIntersection(Rectangle(placement, accessor.getSize()));

	const size_t yb_min = clipping.topLeft.y / 8;
	size_t yb_max = (clipping.bottomRight.y - 1) / 8;

	Point dataTopLeft = this->getSourceOrigin(placement);
	size_t i_start = (dataTopLeft.y / 8) * accessor.getSize().x + dataTopLeft.x;
	const uint8_t lshift_upper = placement.y & (digits - 1); // x & (digits - 1) coresponds x % digits with support for negative int

	if (lshift_upper)
	{
		// Split bytes, shift and reassembly
		if (clipping.bottomRight.y == int16_t(R.y)) yb_max++;
		const uint8_t rshift_lower = 8 - lshift_upper;

		if(this->color.getValue()) {
			for (int16_t x = clipping.topLeft.x; x < clipping.bottomRight.x; x++)
			{
				size_t i = i_start++;
				size_t yb = yb_min;

				if (clipping.topLeft.y > 0)
				{
					this->buffer[yb][x] |= accessor[i] << lshift_upper;
					yb++;
				}
				while (yb < yb_max)
				{
					this->buffer[yb][x] |= accessor[i] >> rshift_lower;
					i += accessor.getSize().x;
					this->buffer[yb][x] |= accessor[i] << lshift_upper;
					yb++;
				}
				if (clipping.bottomRight.y < int16_t(R.y))
					this->buffer[yb][x] |= accessor[i] >> rshift_lower;
			}
		} else {
			for (int16_t x = clipping.topLeft.x; x < clipping.bottomRight.x; x++)
			{
				size_t i = i_start++;
				size_t yb = yb_min;

				if (clipping.topLeft.y > 0)
				{
					this->buffer[yb][x] &= ~(accessor[i] << lshift_upper);
					yb++;
				}
				while (yb < yb_max)
				{
					this->buffer[yb][x] &= ~(accessor[i] >> rshift_lower);
					i += accessor.getSize().x;
					this->buffer[yb][x] &= ~(accessor[i] << lshift_upper);
					yb++;
				}
				if (clipping.bottomRight.y < int16_t(R.y))
					this->buffer[yb][x] &= ~(accessor[i] >> rshift_lower);
			}
		}
	} else if (clipping.topLeft.y < clipping.bottomRight.y)
	{
		// Copy bytes as they are
		if(this->color.getValue()) {
			for (int16_t x = clipping.topLeft.x; x < clipping.bottomRight.x; x++)
			{
				size_t i = i_start++;
				size_t yb = yb_min;

				while (yb < yb_max)
				{
					this->buffer[yb][x] |= accessor[i];
					i += accessor.getSize().x;
					yb++;
				}
				this->buffer[yb][x] |= accessor[i];
			}
		} else {
			for (int16_t x = clipping.topLeft.x; x < clipping.bottomRight.x; x++)
			{
				size_t i = i_start++;
				size_t yb = yb_min;

				while (yb < yb_max)
				{
					this->buffer[yb][x] &= ~accessor[i];
					i += accessor.getSize().x;
					yb++;
				}
				this->buffer[yb][x] &= ~accessor[i];
			}
		}
	}
}