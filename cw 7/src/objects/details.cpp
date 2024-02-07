#include <glew.h>
#include "../Shader_Loader.h"
#include "../Render_Utils.h"
#include "../Texture.h"

class Detail {
private:

	GLuint engineDetail;
	GLuint engineDetailNormal;
	
	GLuint firstAidKit;
	GLuint firstAidKitNormal;

	GLuint crewMember;
	GLuint crewMemberNormal;

	GLuint programTex;

	Core::RenderContext spaceshipEngineDetailContext;
	Core::RenderContext firstAidBoxContext;
	Core::RenderContext crewMemberContext;

	bool isShifWtPressed = false;
	bool isPKeyPressed = false;
	bool canPickUpEngine = false;
	bool canPickUpKit = false;

	bool canPickUpCrew = false;
	bool isRKeyPressed = false;

	bool drawSpaceshipEngine = true;
	bool drawSpaceshipKit = true;
	bool drawSpaceshipCrew = true;

	bool isDragging = false;

	bool isEnginePickedUp = false;
	bool isKitPickedUp = false;
	bool isCrewPickedUp = false;

	bool isEngineCloseToShipRepair = false;
	bool isKitCloseToShipRepair = false;
	bool isCrewCloseToShipRepair = false;






};