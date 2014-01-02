<?php 
require_once "imports.php";
$error = false;

if (isset($_GET) && isset($_GET["a"]))
{
	if ($_GET["a"] == "logout")
	{
		session_start();
		session_destroy();
	}
}

if ($_SERVER['REQUEST_METHOD'] == 'POST')
{
	session_start();

	$username = $_POST["username"];
	$password = $_POST["password"];
		
	if (empty($username) || empty($password))
	{
		$error = true;
	}
	else
	{
		$wi = new WebInterface();
		if (!$wi->isConnected)
		{
			die();
		}
		
		$user = $wi->dbConnection->GetUserByName($username);
		if (!is_null($user))
		{
			if (password_verify($password, $user->passwordhash))
			{
				$_SESSION["currentuser"] = $user->id;
				header("Location: profilepage.php");
			}
			else
			{
				$error = true;
			}
		}
		else
		{
			print 'no user found';
			$error = true;
		}
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
				<h2>Login</h2>
				<div class="logo"><img src="gfx/snes-logo.svg" alt="SNES" /></div>
			</div>

			<div>
			<?php if ($error) { ?><p> Username or Password wrong!</p><?php }?>
			
				<form id="login" action="index.php" method="post">
					<p>
						<label for="username" class="username">Username</label>
						<input type="text" id="username" name="username" value="" autocomplete="off" autofocus required />
					</p>
					<p>
						<label for="password" class="password">Password</label>
						<input type="password" id="password" name="password" required />
					</p>
					<p>
						<button id="clientLogin" name="clientLogin">»&nbsp;&nbsp;Login</button>
					</p>
				</form>
			</div>
		</div>

		<footer>
			<p>Visit the SNESoIP project repository on <a href="https://github.com/mupfelofen-de/SNESoIP">GitHub</a>.</p>
		</footer>

	</body>
</html>
