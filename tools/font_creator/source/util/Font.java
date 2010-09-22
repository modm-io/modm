/*
 * Created on 11.02.2004
 *
 * To change the template for this generated file go to
 * Window&gt;Preferences&gt;Java&gt;Code Generation&gt;Code and Comments
 */
package util;

import java.awt.Rectangle;
import java.awt.image.IndexColorModel;
import java.util.Vector;

/**
 * @author Fabian Maximilian Thiele
 */
public class Font {
	private int bits;
	private int height;
	private int width;
	private int startIndex;
	private int charSpace;

	private int cropTop;
	private int cropBottom;

	private Vector<FontCharacter> chars;
	private String name;

	private IndexColorModel editorColorModel;

	public Font(int width, int initWidth, int height, int startIdx,
			int charCount, String name, int bits, java.awt.Font importFont) {
		chars = new Vector<FontCharacter>(charCount);
		this.width = width;
		this.height = height;
		this.name = name;
		this.startIndex = startIdx;
		this.charSpace = 1;
		this.bits = bits;

		editorColorModel = IndexColorModelUtils.getColorModel(bits);

		if (importFont != null) {
			this.height = height = importFont.getSize() * 2;
		}

		char j = (char) startIndex;
		cropTop = height;
		cropBottom = height;
		for (int i = 0; i < charCount; i++) {
			FontCharacter c = new FontCharacter(initWidth, height, this);
			if (importFont != null) {
				Rectangle rect = c.importChar(importFont, j);
				if (j != ' ') {
					if (cropTop > rect.y)
						cropTop = rect.y;
					if (cropBottom > rect.height)
						cropBottom = rect.height;
				}
				j++;
			}
			chars.addElement(c);
		}

		if (importFont != null) {
			for (int i = 0; i < charCount; i++) {
				((FontCharacter) (chars.get(i))).setSize(-cropTop, -cropBottom,
						0, 0);
			}
		}
	}

	public Font(SerializableFont sFont) {
		width = sFont.getWidth();
		height = sFont.getHeight();
		name = sFont.getName();
		startIndex = sFont.getStartIndex();
		charSpace = sFont.getCharSpace();
		bits = sFont.getBits();
		cropTop = sFont.getCropTop();
		cropBottom = sFont.getCropBottom();

		editorColorModel = IndexColorModelUtils.getColorModel(bits);

		SerializableFontCharacter[] c = sFont.getChars();
		chars = new Vector<FontCharacter>(c.length);
		for (int i = 0; i < c.length; i++) {
			chars.add(c[i].getFontCharacter(this));
		}
	}

	public FontCharacter getChar(int index) {
		return chars.get(index);
	}

	public FontCharacter[] getAllChars() {
		FontCharacter[] c = new FontCharacter[chars.size()];
		for (int i = 0; i < chars.size(); i++) {
			c[i] = chars.get(i);
		}
		return c;
	}

	public void setChar(FontCharacter fontChar, int index) {
		chars.setElementAt(fontChar, index);
	}

	public int getHeight() {
		return height;
	}

	public int getWidth() {
		return width;
	}

	public String getName() {
		return name;
	}

	public void setName(String string) {
		name = string;
	}

	public String toString() {
		return name;
	}

	public int getStartIndex() {
		return startIndex;
	}

	public void setStartIndex(int i) {
		startIndex = i;
	}

	public int getCharCount() {
		return chars.size();
	}

	public void insertCharAt(int index) {
		FontCharacter c = new FontCharacter(width, height, this);
		c.setSize(-cropTop, -cropBottom, 0, 0);
		chars.add(index, c);
	}

	public void removeCharAt(int index) {
		chars.remove(index);
	}

	public int getCharSpace() {
		return charSpace;
	}

	public void setCharSpace(int i) {
		charSpace = i;
	}

	public IndexColorModel getEditorColorModel() {
		return editorColorModel;
	}

	public int getBits() {
		return bits;
	}

	public int getSize() {
		int size = 0;
		for (int i = 0; i < chars.size(); i++)
		{
			FontCharacter fc = chars.get(i);
			int h = fc.getHeight() / 8;
			if (fc.getHeight() % 8 != 0)
				h = h + 1;
			size += fc.getWidth() * h * bits;
		}

		return size;
	}

	public int getCropBottom() {
		return cropBottom;
	}

	public void setCropBottom(int cropBottom) {
		this.cropBottom = cropBottom;
	}

	public int getCropTop() {
		return cropTop;
	}

	public void setCropTop(int cropTop) {
		this.cropTop = cropTop;
	}
}
