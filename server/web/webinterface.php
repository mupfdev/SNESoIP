<?php
/* webinterface.php -*-web-*-
 * 
 * Author: Daniel Baumann
 *
 * This program is part of the SNESoIP project and has has been released
 * under the terms of a BSD-like license.  See the file LICENSE for
 * details. */


class WebInterface
{
	public $dbConnection;
	public $isConnected = false;

	public function __construct()
	{
		$this->dbConnection = new DatabaseConnection(DB_CONN_STRING, DB_USER, DB_PASS);
		$this->isConnected = $this->dbConnection->Connect();
	}
}
?>
