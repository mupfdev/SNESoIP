<?php
/* user.php -*-web-*-
 * 
 * Author: Daniel Baumann
 *
 * This program is part of the SNESoIP project and has has been released
 * under the terms of a BSD-like license.  See the file LICENSE for
 * details. */


class User
{
	public $id;
	public $name;
	public $create_time;
	public $profile;
	public $is_authenticated;
	public $passwordhash;
	public $is_admin;

	public function __construct()
	{

	}
}
?>
