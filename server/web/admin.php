<?php
/* admin.php -*-web-*-
 * 
 * Author: Daniel Baumann
 *
 * This program is part of the SNESoIP project and has has been released
 * under the terms of a BSD-like license.  See the file LICENSE for
 * details. */


require_once "imports.php";
session_start();

if (!isset($_SESSION["currentuser"]))
{
	header("Location: index.php");
}

$wi = new WebInterface();
if (!$wi->isConnected)
{
	die();
}
$user = $wi->dbConnection->GetUserByID($_SESSION["currentuser"]);
if (!$user->is_admin)
{
	header("Location: index.php");
}


$errors = array();

if ($_SERVER['REQUEST_METHOD'] == 'POST')
{
	$action = intval($_POST["action"]);

	switch ($action)
	{
		case 1: // Add new user
			if ($_POST["password"] != $_POST["repassword"])
			{
				$errors[] = "E_WRONGPASSWORD";
			}
			else
			{
				$user = new User();
				$user->name = $_POST["username"];
				$user->passwordhash = password_hash($_POST["password"], PASSWORD_DEFAULT);
				if (isset($_POST["isadmin"]))
				{
					$user->is_admin = $_POST["isadmin"];
				}

				$userid = $wi->dbConnection->AddUser($user);
				if ($userid)
				{
					$profile = new Profile();
					$profile->email = $_POST["email"];
					$profile->region = $_POST["region"];
					$profile->realName = $_POST["realname"];
					$wi->dbConnection->UpdateProfile($profile, $userid);
				}
				else
				{
					$errors[] = "E_USERNOTADDED";
				}
			}
			break;
		case 2:
			// Add Hardware
			$hardware = new Hardware();
			$hardware->hwID = $_POST["hwid"];
			$hardware->currentIP = "";
			$hardware->opponentID = -1;
			$hardware->owner = $_POST["owner"];

			$wi->dbConnection->AddDevice($hardware);
			break;
	}
}
?>
<!doctype html>
<html>
	<head>
		<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
		<meta name="viewport" content="width=device-width, initial-scale=1.0" />
		<title>SNESoIP server</title>
		<link rel="stylesheet" href="style.css" type="text/css" />

		<!--[if lt IE 9]>
		<script src="http://html5shiv.googlecode.com/svn/trunk/html5.js"></script>
		<![endif]-->
	</head>
	<body>

		<div class="box">
			<h1><a href=".">SNESoIP server</a></h1>
			<div>
				<p class="profile"><a href="profilepage.php">Back to Profile</a> <a href="index.php?a=logout">Logout</a></p>
				<h2>Administration</h2>
				<div class="logo"><img src="gfx/snes-logo.svg" alt="SNES" /></div>
			</div>

			<div>
				<h2>Add new user</h2>
				<form id="addUser" action="admin.php" method="post">
					<input type="hidden" name="action" id="action" value="1"/>
					<?php
					if (isset($action) && $action == 1 && count($errors) > 0)
					{
						foreach ($errors as $error)
						{
							echo "<h3>$error</h3>";
						}
					}
					?>
					<p>
						<label for="username" class="username">Username</label>
						<input type="text" id="username" name="username" value="" autocomplete="off" autofocus required />
					</p>
					<p>
						<label for="password" class="password">Password</label>
						<input type="password" id="password" name="password" required />
					</p>
					<p>
						<label for="repassword" class="repassword">Password validation</label>
						<input type="password" id="repassword" name="repassword" required />
					</p>
					<p>
						<label for="isadmin" class="isadmin">Is Admin</label>
						<input type="checkbox" id="isadmin" name="isadmin" value="1"/>
					</p>
					<p>
						<label for="email" class="email">E-Mail</label>
						<input type="text" id="email" name="email" />
					</p>
					<p>
						<label for="realname" class="realname">Realname</label>
						<input type="text" id="realname" name="realname" />
					</p>
					<p>
						<label for="region" class="region">Region</label>
						<input type="text" id="region" name="region" />
					</p>
					<p>
						<button id="addUser" name="addUser">»&nbsp;&nbsp;Add User</button>
					</p>
				</form>
			</div>

			<div>
				<h2>Add new Device</h2>
				<form id="addDevice" action="admin.php" method="post">
					<input type="hidden" name="action" id="action" value="2"/>
					<p>
						<label for="hwid" class="hwid">Hardware ID</label>
						<input type="number" id="hwid" name="hwid" required />
					</p>
					<p>
						<label for="owner" class="owner">Owner</label>
						<select id="owner" name="owner" required>
						<?php
							$users = $wi->dbConnection->GetAllUsers();
							echo HtmlHelper::GetUserComboBoxOptions($users);
						?>
						</select>
					</p>
					<p>
						<button id="addDevice" name="addDevice">»&nbsp;&nbsp;Add Device</button>
					</p>
				</form>
			</div>

		</div>

		<footer>
			<p>Visit the SNESoIP project repository on <a href="https://github.com/mupfelofen-de/SNESoIP">GitHub</a>.</p>
		</footer>

	</body>
</html>
