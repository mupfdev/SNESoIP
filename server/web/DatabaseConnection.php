<?php
/* DatabaseConnection.php -*-web-*-
 * 
 * Author: Daniel Baumann
 *
 * This program is part of the SNESoIP project and has has been released
 * under the terms of a BSD-like license.  See the file LICENSE for
 * details. */


class DatabaseConnection
{
	private $connString;
	private $dbConnection;
	private $user;
	private $pass;

	public function __construct($connString,$user,$pass)
	{
		$this->connString = $connString;
		$this->user = $user;
		$this->pass = $pass;
	}

	public function connect()
	{
		if (is_null($this->connString) || $this->connString == "")
		{
			return false;
		}

		try
		{
			$this->dbConnection = new PDO($this->connString, $this->user, $this->pass);
			if (DEBUG)
			{
				$this->dbConnection->setAttribute( PDO::ATTR_ERRMODE, PDO::ERRMODE_WARNING );
			}
			return true;
		}
		catch (PDOException $e)
		{
			print "Could not connect to database";
			print $e->getMessage();
			return false;
		}
	}

	public function AddUser(User $user)
	{
		try
		{

			// 1st Step: Create an empty profile for the new user
			$statement = $this->dbConnection->prepare("INSERT INTO profile (region) VALUES ('unknown')");
			$statement->execute();
			$profileID = $this->dbConnection->lastInsertId() ;

			// 2nd Step: Create the user
			$statement = $this->dbConnection->prepare("INSERT INTO user (username,password,admin,profile_idprofile) VALUES (:username,:password,:admin,:profileid)");

			if (!$statement)
			{
    			echo "\nPDO::errorInfo():\n";
    			print_r($dbh->errorInfo());
    			die();
			}

			$statement->bindParam(":username",$user->name);
			$statement->bindParam(":password",$user->passwordhash);
			$statement->bindParam(":admin",$user->is_admin);
			$statement->bindParam(":profileid",$profileID);

			if ($statement->execute())
			{
				return $this->dbConnection->lastInsertId();
			}
			else
			{
				return false;
			}
		}
		catch (PDOException $e)
		{
			print "Could not add user";
			print $e->getMessage();

		}
	}

	public function AddDevice($hardware)
	{
		try
		{
			$statement = $this->dbConnection->prepare("INSERT INTO snesoip_hw (hwid, user_userid, hwid_opponent, currentip) VALUES (:hwid,:userid,:opponentid,:currentip)");
			$statement->bindParam(":hwid", $hardware->hwID);
			$statement->bindParam(":opponentid", $hardware->opponentID);
			$statement->bindParam(":userid", $hardware->owner);
			$statement->bindParam(":currentip", $hardware->currentIP);

			$statement->execute();
		}
		catch (PDOException $e)
		{
			print "Could not add user";
			print $e->getMessage();

		}
	}

	public function SetPassword($userid, $newPasswordHash)
	{
		try
		{
			$statement = $this->dbConnection->prepare("UPDATE user SET password = :password WHERE userid = :userid");

			$statement->bindParam(":password",$newPasswordHash);
			$statement->bindParam(":userid",$userid);

			if ($statement->execute())
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		catch (PDOException $e)
		{
			print "Could not get user";
			print $e->getMessage();
			return false;
		}

	}

	public function GetListOfAllDevices($excludeOwn = false, $ownid = 0)
	{
		try
		{
			if ($excludeOwn)
			{
				$statement = $this->dbConnection->prepare("SELECT snesoip_hw.hwid, snesoip_hw.hwid_opponent, snesoip_hw.currentip, user.username FROM snesoip_hw INNER JOIN user ON snesoip_hw.user_userid = user.userid WHERE snesoip_hw.hwid != :ownid");
				$statement->bindParam(":ownid", $ownID);
			}
			else
			{
				$statement = $this->dbConnection->prepare("SELECT snesoip_hw.hwid, snesoip_hw.hwid_opponent, snesoip_hw.currentip, user.username FROM snesoip_hw INNER JOIN user ON snesoip_hw.user_userid = user.userid");
			}


			if ($statement->execute())
			{
				$hardware = array();
				$i = 0;
				while ($row = $statement->fetch(PDO::FETCH_ASSOC))
				{
					$hw = new Hardware();
					$hw->currentIP = $row["currentip"];
					$hw->hwID = $row["hwid"];
					$hw->opponentID = $row["hwid_opponent"];
					$hardware[$i]["hw"] = $hw;
					$hardware[$i]["owner"] = $row["username"];
					$i++;
				}
				return $hardware;
			}
			else
			{
				return null;
			}
		}
		catch (PDOException $e)
		{
			print "Could not get list of user hardware";
			print $e->getMessage();
		}

	}

	public function GetUserDevices($userid)
	{
		try
		{
			$statement = $this->dbConnection->prepare("SELECT snesoip_hw.hwid, snesoip_hw.hwid_opponent, snesoip_hw.currentip FROM snesoip_hw WHERE user_userid = :userid");
			$statement->bindParam(":userid", $userid);

			if ($statement->execute())
			{
				$hardware = array();
				while ($row = $statement->fetch(PDO::FETCH_ASSOC))
				{
					$hw = new Hardware();
					$hw->currentIP = $row["currentip"];
					$hw->hwID = $row["hwid"];
					$hw->opponentID = $row["hwid_opponent"];
					$hardware[] = $hw;
				}
				return $hardware;
			}
			else
			{
				return null;
			}
		}
		catch (PDOException $e)
		{
			print "Could not get list of user hardware";
			print $e->getMessage();
		}
	}

	public function GetUserByName($username)
	{
		try
		{
			$statement = $this->dbConnection->prepare("SELECT * FROM user RIGHT JOIN profile ON user.profile_idprofile = profile.idprofile WHERE username = :username");
			$statement->bindParam(":username",$username);

			if ($statement->execute())
			{
				if ($row = $statement->fetch(PDO::FETCH_ASSOC))
				{
					$user = new User();
					$user->id = $row["userid"];
					$user->name = $row["username"];
					$user->create_time = $row["create_time"];
					$user->passwordhash = $row["password"];
					$user->is_admin = $row["admin"];

					$user->profile = new Profile();
					$user->profile->realName = $row["realname"];
					$user->profile->region = $row["region"];
					$user->profile->email = $row["email"];

					return $user;
				}
				else
				{
					return null;
				}
			}
			else
			{
				return null;
			}
		}
		catch (PDOException $e)
		{
			print "Could not get user";
			print $e->getMessage();
		}
	}

	public function GetUserByID($userid)
	{
		try
		{
			$statement = $this->dbConnection->prepare("SELECT * FROM user RIGHT JOIN profile ON user.profile_idprofile = profile.idprofile WHERE userid = :userid");
			$statement->bindParam(":userid",$userid);

			if ($statement->execute())
			{
				if ($row = $statement->fetch(PDO::FETCH_ASSOC))
				{
					$user = new User();
					$user->id = $userid;
					$user->name = $row["username"];
					$user->create_time = $row["create_time"];
					$user->passwordhash = $row["password"];
					$user->is_admin = $row["admin"];

					$user->profile = new Profile();
					$user->profile->realName = $row["realname"];
					$user->profile->region = $row["region"];
					$user->profile->email = $row["email"];

					return $user;
				}
				else
				{
					return null;
				}
			}
			else
			{
				return null;
			}
		}
		catch (PDOException $e)
		{
			print "Could not get user";
			print $e->getMessage();
		}
	}

	public function GetProfileID($userid)
	{
		try
		{
			$statement = $this->dbConnection->prepare("SELECT profile_idprofile FROM user WHERE userid = :userid");
			$statement->bindParam(":userid",$userid);

			if ($statement->execute())
			{
				if ($row = $statement->fetch(PDO::FETCH_ASSOC))
				{
					return $row["profile_idprofile"];
				}
				else
				{
					return null;
				}
			}
			else
			{
				return null;
			}
		}
		catch (PDOException $e)
		{
			print "Could not get user";
			print $e->getMessage();
		}
	}

	public function UpdateProfile($profile, $userid)
	{
		try
		{
			$profileID = $this->GetProfileID($userid);
			if (!is_null($profileID))
			{
				$statement = $this->dbConnection->prepare("UPDATE profile SET realname = :realname, avatar = :avatar, region = :region, email = :email WHERE idprofile = :idprofile");
				$statement->bindParam(":realname",$profile->realName);
				$statement->bindParam(":avatar",$profile->avatar);
				$statement->bindParam(":region",$profile->region);
				$statement->bindParam(":email",$profile->email);
				$statement->bindParam(":idprofile",$profileID);

				if ($statement->execute())
				{
					return true;
				}
				else
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}
		catch (PDOException $e)
		{
			print "Could not get user";
			print $e->getMessage();
			return false;
		}
	}

	public function GetAllUsers()
	{
		try
		{
			$statement = $this->dbConnection->prepare("SELECT userid, username FROM user");

			if ($statement->execute())
			{
				$users = array();
				while ($row = $statement->fetch(PDO::FETCH_ASSOC))
				{
					$user = new User();
					$user->id = $row["userid"];
					$user->name = $row["username"];
					$users[] = $user;
				}
				return $users;
			}
			else
			{
				return null;
			}
		}
		catch (PDOException $e)
		{
			print "Could not get user";
			print $e->getMessage();
			return false;
		}
	}

	public function SetOpponent($hwID, $opponentID)
	{
		try
		{
			$statement = $this->dbConnection->prepare("UPDATE snesoip_hw SET hwid_opponent = :opponent WHERE hwid = :hwid");
			$statement->bindParam(":opponent",$opponentID);
			$statement->bindParam(":hwid",$hwID);

			if ($statement->execute())
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		catch (PDOException $e)
		{
			print "Could not get user";
			print $e->getMessage();
			return false;
		}
	}

	public function GetAdminUsers()
	{
		try
		{
			$statement = $this->dbConnection->prepare("SELECT userid, username FROM user WHERE admin = 1");

			if ($statement->execute())
			{
				$users = array();
				while ($row = $statement->fetch(PDO::FETCH_ASSOC))
				{
					$user = new User();
					$user->id = $row["userid"];
					$user->name = $row["username"];
					$users[] = $user;
				}
				return $users;
			}
			else
			{
				return null;
			}
		}
		catch (PDOException $e)
		{
			print "Could not get user";
			print $e->getMessage();
			return null;
		}
	}

	public function CreateTables()
	{
		try
		{
			$statement = $this->dbConnection->prepare("CREATE TABLE IF NOT EXISTS `snesoip`.`profile` (`idprofile` INT NOT NULL AUTO_INCREMENT, `realname` VARCHAR(255) NULL, `avatar` BLOB NULL, `region` VARCHAR(45) NULL, `email` VARCHAR(255) NULL, PRIMARY KEY (`idprofile`), UNIQUE INDEX `idprofile_UNIQUE` (`idprofile` ASC)) ENGINE = InnoDB;");
			if ($statement->execute())
			{
				$statement = $this->dbConnection->prepare("CREATE TABLE IF NOT EXISTS `snesoip`.`user` (`userid` INT NOT NULL AUTO_INCREMENT,`username` VARCHAR(32) NOT NULL,`password` VARCHAR(255) NOT NULL,`create_time` TIMESTAMP NULL DEFAULT CURRENT_TIMESTAMP,`admin` TINYINT NULL,`profile_idprofile` INT NOT NULL, UNIQUE INDEX `hwid_UNIQUE` (`userid` ASC), UNIQUE INDEX `username_UNIQUE` (`username` ASC), PRIMARY KEY (`userid`), INDEX `fk_user_profile_idx` (`profile_idprofile` ASC), CONSTRAINT `fk_user_profile` FOREIGN KEY (`profile_idprofile`)  REFERENCES `snesoip`.`profile` (`idprofile`)  ON DELETE NO ACTION  ON UPDATE NO ACTION);");
				if ($statement->execute())
				{
					$statement = $this->dbConnection->prepare("CREATE TABLE IF NOT EXISTS `snesoip`.`snesoip_hw` (`hwid` INT NOT NULL,`user_userid` INT NOT NULL, `hwid_opponent` INT NULL, `currentip` VARCHAR(128) NULL, `lastping` VARCHAR(45) NULL, PRIMARY KEY (`hwid`, `user_userid`), INDEX `fk_snesoip_hw_user1_idx` (`user_userid` ASC), CONSTRAINT `fk_snesoip_hw_user1`   FOREIGN KEY (`user_userid`)   REFERENCES `snesoip`.`user` (`userid`)   ON DELETE NO ACTION   ON UPDATE NO ACTION) ENGINE = InnoDB;");
					if ($statement->execute())
					{
						return true;
					}
					else
					{
						return false;
					}
				}
				else
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}
		catch (PDOException $e)
		{
			print "Could not get user";
			print $e->getMessage();
			return false;
		}
	}
}
?>
