<?php
/* install.php -*-web-*-
 * Install script.
 * Author: Daniel Baumann
 *
 * This program is part of the SNESoIP project and has has been released
 * under the terms of a BSD-like license.  See the file LICENSE for
 * details. */

require_once 'webinterface.php';
require_once 'DatabaseConnection.php';
require_once 'user.php';
require_once 'password.php';

$errors = array();
$alreadyInstalled = false;

$dbConnString = "";
$dbuser = "";
$dbpass = "";

if (file_exists("configuration.php"))
{
	$alreadyInstalled = true;
}

if (!$alreadyInstalled && $_SERVER['REQUEST_METHOD'] == 'POST')
{
	if (!empty($_POST["password"]) && !empty($_POST["repassword"]) && $_POST["password"] == $_POST["repassword"])
	{
		define('DB_CONN_STRING',$_POST["dbConnString"]);
		define('DB_USER',$_POST["dbusername"]);
		define('DB_PASS',$_POST["dbpassword"]);
		define('DEBUG', true);
		$dbConnString = DB_CONN_STRING;
		$dbuser = DB_USER;
		$dbpass = DB_PASS;

		$wi = new WebInterface();
		if ($wi->isConnected)
		{
			$configFileContent = "<?php\n// Database connection string\ndefine('DB_CONN_STRING','{CONN_STRING}');\n\n// Database user\ndefine('DB_USER','{DB_USER}');\n\n// Databse password\ndefine('DB_PASS','{DB_PASS}');\n\n// Send debug data to the browser\ndefine('DEBUG', false);?>";
			$configFileWriteable = false;

			if (is_writable("."))
			{
				$configFileWriteable = true;
			}

			// 1st step: write the config file
			$configFileContent = str_ireplace("{CONN_STRING}", DB_CONN_STRING, $configFileContent);
			$configFileContent = str_ireplace("{DB_USER}", DB_USER, $configFileContent);
			$configFileContent = str_ireplace("{DB_PASS}", DB_PASS, $configFileContent);


			if ($wi->dbConnection->CreateTables())
			{

				$user = new User();
				$user->name = $_POST["username"];
				$user->passwordhash = password_hash($_POST["password"], PASSWORD_DEFAULT);
				$user->is_admin = 1;
				if ($wi->dbConnection->AddUser($user))
				{
					if ($configFileWriteable)
					{
						file_put_contents("configuration.php", $configFileContent);
					}
					$alreadyInstalled = true;
				}
				else
				{
					$errors[] = "E_COULDNOTADDADMIN";
				}
			}
			else
			{
				$errors[] = "E_COULDNOTCREATETABLES";
			}
		}
		else
		{
			$errors[] = "E_NODATABASECONNECTION";
		}
	}
	else
	{
		$errors[] = "E_PASSWORDSDONOTMATCH";
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
			<h1><a href=".">SNESoIP server Installation</a></h1>
			<?php if ($alreadyInstalled) { ?>
				<div>
					<h2>Congratulations, you can now login <a href="index.php" title="Homepage">here</a></h2>
					<?php if (isset($configFileWriteable) && $configFileWriteable == false) {?>
					<h2>Warning: The configuration file could not be written automatically! Please create a file called 'configuration.php' with the following content: </h2>
					<textarea id="configcontent"><?php echo $configFileContent; ?></textarea>
					<?php }?>
					<p>Delete the 'configuration.php' file if you want to run the installation again!</p>
					<div class="logo"><img src="gfx/snes-logo.svg" alt="SNES" /></div>
				</div>
			<?php } else {
				if (count($errors) > 0)
				{
					foreach ($errors as $error)
					{
						print "<h3>".$error."</h3>";
					}
				}

			?>
			<form id="install" action="install.php" method="post">
			<div>
				<h2>Database settings</h2>
				<div class="logo"><img src="gfx/snes-logo.svg" alt="SNES" /></div>
			</div>
			<div>
					<p>
						<label for="host" class="dbConnString">Host</label>
						<small>Example: 'mysql:host=localhost;dbname=snesoip;charset=utf8'. See PHP-PDO manual for details</small>
						<input type="text" id="dbConnString" name="dbConnString" value="<?php echo $dbConnString; ?>" autocomplete="off" autofocus required />
					</p>
					<p>
						<label for="dbusername" class="dbusername">Username</label>
						<input type="text" id="dbusername" name="dbusername" value="<?php echo $dbuser; ?>" autocomplete="off" autofocus required />
					</p>
					<p>
						<label for="password" class="dbpassword">Password</label>
						<input type="password" id="dbpassword" name="dbpassword" value="<?php echo $dbpass; ?>" required />
					</p>
			</div>

			<div>
				<h2>Admin user</h2>
				<div class="logo"><img src="gfx/snes-logo.svg" alt="SNES" /></div>
			</div>
			<div>
					<p>
						<label for="username" class="username">Username</label>
						<input type="text" id="username" name="username" value="" autocomplete="off" autofocus required />
					</p>
					<p>
						<label for="password" class="password">Password</label>
						<input type="password" id="password" name="password" required />
					</p>
					<p>
						<label for="password" class="repassword">Verify Password</label>
						<input type="password" id="repassword" name="repassword" required />
					</p>
			</div>

			<p>
				<button id="installWI" name="installWI">»&nbsp;&nbsp;Install</button>
			</p>
			</form>
			<?php } ?>
		</div>

		<footer>
			<p>Visit the SNESoIP project repository on <a href="https://github.com/mupfelofen-de/SNESoIP">GitHub</a>.</p>
		</footer>

	</body>
</html>
