/*****************************************************************************
*                                                                            *
*  OpenNI 1.x Alpha                                                          *
*  Copyright (C) 2012 PrimeSense Ltd.                                        *
*                                                                            *
*  This file is part of OpenNI.                                              *
*                                                                            *
*  Licensed under the Apache License, Version 2.0 (the "License");           *
*  you may not use this file except in compliance with the License.          *
*  You may obtain a copy of the License at                                   *
*                                                                            *
*      http://www.apache.org/licenses/LICENSE-2.0                            *
*                                                                            *
*  Unless required by applicable law or agreed to in writing, software       *
*  distributed under the License is distributed on an "AS IS" BASIS,         *
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  *
*  See the License for the specific language governing permissions and       *
*  limitations under the License.                                            *
*                                                                            *
*****************************************************************************/
package org.openni;

/**
 * Stores the data generated by a Hand Event. <BR><BR>
 *
 * Provides a structure for storing the data in Hand Events. <BR><BR>
 *
 * This class allows for the easy storage of the various data values passed
 * along with a Hand events when using the HandGenerator and the NiTE HandPoint APIs.<BR><BR>
 *
 * Normally this class would only be created when a Hand Event is initiated by the system.
 * Users detecting this event would primarily use this API to read the values back out.  <BR><BR>
 *
 * More information on the HandPoint APIs can be found in the NiTE Algorithms document,
 * included in PDF format in the documentation directory when installing PrimeSense NiTE
 *
 */
public class ActiveHandEventArgs extends EventArgs
{
	/**
	 * Constructor to create a new object with defined id, position, and time
	 * @param id ID of the handpoint that triggered this event
	 * @param position Position of the hand in space when event was triggered
	 * @param time Timestamp of the frame that generated this event
	 */
	public ActiveHandEventArgs(int id, Point3D position, float time)
	{
		this.id = id;
		this.position = position;
		this.time = time;
	}
	
	/**
	 * Getter function for the ID
	 * @return ID of the handpoint that generated this event
	 */
	public int getId()
	{
		return this.id;
	}
	/**
	 * Getter function for the position of the hand that generated this event
	 * @return Coordinates of the hand in real world coordinates
	 */
	public Point3D getPosition()
	{
		return this.position;
	}
	/**
	 * Getter function for the time
	 * @return Timestamp of the frame that triggered this event
	 */
	public float getTime()
	{
		return this.time;
	}
	
	
	private final int id;
	private final Point3D position;
	private final float time;
}