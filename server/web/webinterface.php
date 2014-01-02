<?php
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
