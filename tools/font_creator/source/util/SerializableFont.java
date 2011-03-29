/*
 * Created on 29.03.2004
 *
 * To change the template for this generated file go to
 * Window&gt;Preferences&gt;Java&gt;Code Generation&gt;Code and Comments
 */
package util;

import java.io.LineNumberReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.Serializable;
import java.util.ArrayList;

/**
 * @author Fabian Maximilian Thiele
 * @author Fabian Greif
 */
public class SerializableFont implements Serializable {
	private static final long serialVersionUID = 20110327L;
	private int bits;
	private int height;
	private int width;
	private int startIndex;
	private int hspace;
	private int vspace;
	private String name;
	private SerializableFontCharacter[] chars;

	public SerializableFont(Font font) {
		bits = font.getBits();
		height = font.getHeight();
		width = font.getWidth();
		startIndex = font.getStartIndex();
		hspace = font.getHorizontalSpace();
		vspace = font.getVerticalSpace();
		name = font.getName();
		
		FontCharacter[] c = font.getAllCharacters();
		chars = new SerializableFontCharacter[c.length];
		for (int i = 0; i < chars.length; i++) {
			chars[i] = new SerializableFontCharacter(c[i]);
		}
	}
	
	public SerializableFont(File file) throws IOException {
		LineNumberReader reader = new LineNumberReader(new FileReader(file));
		
		// TODO move these values to the font-file
		bits = 1;
		
		name = null;
		width = -1;
		height = -1;
		hspace = -1;
		vspace = -1;
		startIndex = -1;
		int lastIndex = -1;
		
		ArrayList<SerializableFontCharacter> list = new ArrayList<SerializableFontCharacter>();
		String line = null;
		while ((line = reader.readLine()) != null) {
			if (!line.startsWith("#")) {
				continue;
			}
			
			String[] output = line.split(":");
			String key = output[0].trim();
			String value = output[1].trim();
			
			if (key.equals("#font")) {
				name = value;
			}
			else if (key.equals("#width")) {
				width = Integer.valueOf(value);
			}
			else if (key.equals("#height")) {
				height = Integer.valueOf(value);
			}
			else if (key.equals("#hspace")) {
				hspace = Integer.valueOf(value);
			}
			else if (key.equals("#vspace")) {
				vspace = Integer.valueOf(value);
			}
			else if (key.equals("#char")) {
				String[] t = value.split("'");
				int index = Integer.valueOf(t[0].trim());
				
				if (startIndex == -1) {
					startIndex = index;
					lastIndex = index;
				}
				else if (lastIndex + 1 != index) {
					throw new IOException("Unexpected identifier for character " + 
							index + ". Expected " + (lastIndex + 1) + "");
				}
				list.add(new SerializableFontCharacter(reader, index, height));
				lastIndex = index;
			}
			else {
				throw new IOException("Unknown format '" + key + "' in line " + reader.getLineNumber());
			}
		}
		
		if (name == null || width == -1 || height == -1 || hspace == -1) {
			throw new IOException("Not all needed information given!");
		}
		
		chars = (SerializableFontCharacter[]) list.toArray(new SerializableFontCharacter[list.size()]);
		reader.close();
	}
	
	public int getBits() {
		return bits;
	}

	public SerializableFontCharacter[] getChars() {
		return chars;
	}

	public int getHorizontalSpace() {
		return hspace;
	}
	
	public int getVerticalSpace() {
		return vspace;
	}

	public int getHeight() {
		return height;
	}

	public String getName() {
		return name;
	}

	public int getStartIndex() {
		return startIndex;
	}

	public int getWidth() {
		return width;
	}
	
	public void writeToFile(File selectedFile) throws IOException {
		BufferedWriter writer = new BufferedWriter(new FileWriter(selectedFile));
		
		writer.write("#font   : " + name + "\n");
		writer.write("#width  : " + width + "\n");
		writer.write("#height : " + height + "\n");
		writer.write("#hspace : " + hspace + "\n");
		writer.write("#vspace : " + vspace + "\n");
		
		for (int i = 0; i < chars.length; i++) {
			chars[i].writeToFile(writer);
		}
		
		writer.close();
	}
}
